<?php

/**
 * Curs
 * @author agalindo
 *
 */
class Grade {
	private $id        = -1;
	private $name      = "";		
	private $enabled   = FALSE;
	
	public function __construct($aId, $aName, $aEnabled) {
// echo "Grade constructor amb id=$aId, nom = $aName i enabled=$aEnabled\n";
		$this->id      = $aId;
		$this->name    = $aName;
		$this->enabled = $aEnabled;
	}

	public function __destruct() {
		
	}

	public function __toString() {
		$enabledTxt = $this->isEnabled() ? "habilitat" : "deshabilitat";
		return "Curs ". $this->getName() . " (" . $this->getId() . ") " . $enabledTxt;
	}

	public function getName() {
		return $this->name;
	}

	public function getId() {
		return $this->id;
	}
	
	public function isEnabled() {
		return $this->enabled;
	}
	
}

?>
