<?php

class User {
	private $id       = -1;
	private $mail     = "";
	private $fullname = "";
	private $admin  = FALSE;
		

	public function __construct($aId, $aMail, $aFullname) {
		$this->id      =$aId;
		$this->mail    =$aMail;
		$this->fullname=$aFullname;
		$admins = explode(" ", $GLOBALS['appAdmins']);
		foreach ($admins as $anAdmin) {
			if ($this->mail === $anAdmin) {
				$this->admin = TRUE;
			}
		}
	}

	public function __destruct() {
		
	}

	public function __toString() {
		$t = $this->isAdmin()? " administrador " : " ";
		return "Usuari". $t . $this->getFullname() . ", amb mail " . $this->getMail() . ", id " . $this->getId();
	}

	public function getFullname() {
		return $this->fullname;
	}
	
	public function getMail() {
		return $this->mail;
	}

	public function getId() {
		return $this->id;
	}

	public function isAdmin() {
		return $this->admin;
	}
	
	
}

?>
