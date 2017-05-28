<?php

/**
 * Notifyme (voler ser notificat sobre nous anuncis de tal curs)
 * @author agalindo
 *
 */
class NotifyMe {
	private $id      = -1;
	private $owner   = "";		
	private $grade   = -1;
	
	public function __construct($aId, $aOwner, $aGrade) {
		$this->id    = $aId;
		$this->owner = $aOwner;
		$this->grade = $aGrade;
	}

	public function __destruct() {
		
	}

	public function __toString() {
		return "Desig de l'usuari " . $this->getOwner() . " de ser notificat sobre anuncis de llibres del curs " . $this->getGrade();
	}

	public function getId() {
		return $this->id;
	}
	
	public function getOwner() {
		return $this->owner;
	}

	public function getGrade() {
		return $this->grade;
	}
	
}

?>
