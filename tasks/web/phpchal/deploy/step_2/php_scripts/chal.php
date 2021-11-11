<?php

if (count($argv) == 1) {
    die("Not enought params");
}
$param = $argv[1];

$body = explode(".", $param, 3);

if (count($body) !== 3) {
    die("missmatched body");
}

$first = $body[0];
if ($first !== '❤️php') {
    die("Heart attack!");
}

$second = $body[1];
if ($second == "577869247350" || hash('tiger192,3', $second) != "0e6918727449899603894527213587507719940455533277") {
    die("Nope. That can't be so easy");
}

$last = $body[2]; 
if (strlen($last) == strlen(strtoupper($last))) {
   die("$last is invalid");
}

print_r(getenv());



