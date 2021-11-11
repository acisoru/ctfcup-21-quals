<?php

if (count($argv) == 1) {
    die("Not enought params");
}

$answer = $argv[1];

if ($answer === getenv("ANSWER")) {
    echo file_get_contents("/flag");
} else {
    die("No flag for loosers");
}
