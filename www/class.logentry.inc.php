<?php

class LogEntry {
	private $timestamp   = NULL;
	private $action      = NULL;
	private $user        = NULL;
	private $description = NULL;
	
	public function __construct($aTimestamp, $aAction, $aUser, $aDescription) {
		$this->timestamp   = $aTimestamp;
		$this->action      = $aAction;
		$this->user        = $aUser;
		$this->description = $aDescription;
	}

	public function __destruct() {
		
	}

	public function __toString() {
		return "Log del " . $this->timestamp . " d'acció " . $this->action . " de l usuari " . $this->user . ", descripció " . $this->description;
	}

/*
	public function getUser() {
		return $this->user;
	}

	/**
	 * Mostra la capçalera d'una taula d'anuncis d'altri
	 * @return string
	 */
	public static function getHtmlHeader() {
		return "<table border=\"1\">"
				. "<tr bgcolor=\"#D0D0D0\">"
				. "<td>Timestamp</td>"
				. "<td>Acci&oacute;</td>"
				. "<td>Usuari</td>"
				. "<td>Descripció</td>"
				. "</tr>";
	}
	
	
	public static function getHtmlFooter() {
		return "</table>";
	}
	

	/**
	 * Mostra una fila d'una taula d'interesos en anunci 
	 * @return string
	 */
	public function toHtmlRow() {
		return "<tr>"
				. LogEntry::toHtmlCell($this->timestamp)
				. LogEntry::toHtmlCell($this->action)
				. LogEntry::toHtmlCell($this->user)
				. LogEntry::toHtmlCell($this->description)
				. "</tr>";
	}
	
	private static function toHtmlCell($text) {
		return "<td bgcolor=\"#F2F2F2\">" . $text . "</td>";
	}
	
	public static function showLogEntryPresentation($numEntries) {
		return "<p>Volcat dels $numEntries logs de l'aplicació:</p>\n";
	}
	
}


?>
