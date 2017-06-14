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
		return "
<table border=1>
<tr><td>0</td><td>ACTION_NONE</td></tr>
<tr><td>2</td><td>ACTION_NEW_AD_FORM</td></tr>
<tr><td>3</td><td>ACTION_SEARCH_BY_ISBN</td></tr>
<tr><td>4</td><td>ACTION_SEARCH_BY_KEYWORDS</td></tr>
<tr><td>5</td><td>ACTION_NEW_AD_PREVIEW</td></tr>
<tr><td>6</td><td>ACTION_SHOW_MY_ADS</td></tr>
<tr><td>8</td><td>ACTION_DELETE_ADD</td></tr>
<tr><td>9</td><td>ACTION_BE_INTERESTED_IN_ADD</td></tr>
<tr><td>10</td><td>ACTION_BE_INTERESTED_IN_ADD_DO</td></tr>
<tr><td>12</td><td>ACTION_RESTORE</td></tr>
<tr><td>13</td><td>ACTION_SEE_NOTIFICATIONS</td></tr>
<tr><td>14</td><td>ACTION_SEARCH_BY_GRADE</td></tr>
<tr><td>15</td><td>ACTION_SHOW_LOGS</td></tr>
</table>
		<p>Volcat dels $numEntries logs de l'aplicació:</p>\n";
	}
	
}


?>
