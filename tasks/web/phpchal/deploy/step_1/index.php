<?php

// Round 1. Lets'go!

if (isset($_GET['payload'])) {
    $payload = json_decode($_GET['payload']);
    if ($payload->password != getenv('SECRET_PASSWORD')) {
        die("Invalid password");
    }
} else {
    show_source(__FILE__);
    exit;
}

$key = getenv('SECRET_KEY');
if (isset($payload->hashKey)) {
    $key = hash_hmac('haval160,4', $payload->hashKey, getenv('SECRET_KEY'));
}

$hash = hash_hmac('haval160,4', $payload->data, $key);
if ($hash !== $payload->hash) {
    die("Invalid hash");
}

class Access {
    protected $username;
    protected $role;
    protected $isAdmin;
    public $password;

    function __construct($user, $password) {
        $this->username = $user; 
        if ($this->username == "admin") {
            die("Can't construct admin");
        }
        $this->role = "regular";
        $this->password = $payload;
        $this->isAdmin = false;
        $this->load();
    }

    public function load()
    {
        if ($this->role === "admin" && $this->username === "admin") {
            if (strcmp($this->password, $ADMIN_PASSWORD) == 0) {
                $this->isAdmin = true;
            }
        }
    }

    public function IsAdmin()
    {
        return $this->isAdmin === true;
    }

    public function SetAdmin()
    {
        $this->role = "admin";
        $this->username = "admin";
    }
}

$acc = unserialize(base64_decode($payload->access));
if ($acc->IsAdmin()) {
    echo '<a href="' . getenv("REDIRECT_URL") . '">Go to round 2!</a>'; 
    exit;
} else {
    die("Nope. You are not admin");
}



