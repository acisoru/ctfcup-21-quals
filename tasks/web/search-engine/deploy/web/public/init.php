<?php

require_once "core.php";

$ch = getCH();
$db = getDB();
initCH($ch);
initDB($db);
chmod('/tmp/mysqlitedb.db', 766);

$userID = registerUser($db, "admin", getenv("ADMIN_PASSWORD"));
addDocument($userID, "flag", getenv("FLAG"));

