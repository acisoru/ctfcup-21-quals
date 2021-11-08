<?php

require __DIR__ . '/../vendor/autoload.php';


function getDB()
{
    return new SQLite3('/tmp/mysqlitedb.db');
}

function getCH()
{
    $config = [
        'host' => 'chouse',
        'port' => '8123',
        'username' => 'default',
        'password' => ''
    ];
    $db = new ClickHouseDB\Client($config);
    $db->database('default');
    $db->setTimeout(2);      // 1500 ms
    $db->setConnectTimeOut(5); // 5 seconds
    return $db;
}

function getMS()
{
    return new MeiliSearch\Client('http://melisearch:7700', 'willbechanged');
}

function initDB($db)
{
    $db->exec('CREATE TABLE IF NOT EXISTS users (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, username VARCHAR(200) UNIQUE, password VARCHAR(200))');
}

function initCH(\ClickHouseDB\Client $ch)
{
    $ch->write('
    CREATE TABLE IF NOT EXISTS queries (
        search_date Date DEFAULT toDate(search_time),
        search_time DateTime,
        query String,
        from IPv4,
        platform String,
        search_index Int64
    )  ENGINE = MergeTree() ORDER BY (search_time, search_index) ');
}

function registerUser(SQLite3 $db, $username, $password)
{
    $prep = $db->prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    $prep->bindParam(':username', $username, SQLITE3_TEXT);
    $prep->bindParam(':password', $password, SQLITE3_TEXT);
    $prep->execute();
    return $db->lastInsertRowID();
}

function loginUser(SQLite3 $db, $username, $password)
{
    $prep = $db->prepare("SELECT id FROM users WHERE username = :username AND password = :password");
    $prep->bindParam(":username", $username, SQLITE3_TEXT);
    $prep->bindParam(":password", $password, SQLITE3_TEXT);
    $res = $prep->execute();
    return $res->fetchArray();
}

function getToken(SQLite3 $db, $id)
{
    $prep = $db->prepare("SELECT * FROM users WHERE id = :id");
    $prep->bindParam(":id", $id, SQLITE3_INTEGER);
    $res = $prep->execute();
    $res = $res->fetchArray();
    if (empty($res)) {
        return '';
    }
    return sha1($res['username'] . $res['password'] . $res['id']);
}

function addDocument($userID, $title, $text)
{
    $docs = [
        ["id" => uniqid(), "title" => $title, "body" => $text],
    ];
    $ms = getMS();
    $msIndex = $ms->index("index_${userID}");
    $msIndex->addDocuments($docs);

}


function logQuery($query, $ips, $plt, $si)
{
    $ch = getCH();
    $ch->insert('queries',
        [
            [time(), $query, $ips, $plt, $si],
        ], ['search_time', 'query', 'from', 'platform', 'search_index']
    );
}

function hottestQueries(ClickHouseDB\Client $ch, $si, $ip = null, $plt = null)
{
    $q = 'SELECT query, cnt FROM (SELECT COUNT(*) as cnt, query as query FROM queries WHERE search_index = ' . $si;
    if ($ip) {
        $q .= ' AND from = \'' . $ip . '\' ';
    }
    if ($plt) {
        $q .= ' AND platform = \'' . $plt . '\' ';
    }
    $q .= ' GROUP BY query) ORDER BY cnt DESC LIMIT 5';

    $st = $ch->select($q);
    return $st->rows();
}

function mostPlatforms(ClickHouseDB\Client $ch, $si)
{

    $st = $ch->select('SELECT cnt, plt FROM (SELECT COUNT(*) as cnt, platform as plt FROM queries WHERE search_index = ' . $si . ' GROUP BY platform) ORDER BY cnt DESC LIMIT 5');
    return $st->rows();
}

function mostIPs(ClickHouseDB\Client $ch, $si)
{
    $st = $ch->select('SELECT cnt, fip FROM (SELECT COUNT(*) as cnt, from as fip FROM queries WHERE search_index = ' . $si . ' GROUP BY from) ORDER BY cnt DESC LIMIT 5');
    return $st->rows();
}

function getPlatform($ua)
{
    $platform = 'Unknown';

    // First get the platform?
    if (preg_match('/linux/i', $ua)) {
        $platform = 'linux';
    } elseif (preg_match('/macintosh|mac os x/i', $ua)) {
        $platform = 'mac';
    } elseif (preg_match('/windows|win32/i', $ua)) {
        $platform = 'windows';
    }
    return $platform;
}
