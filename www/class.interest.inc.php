<?php

class Interest {
	private $id     = -1;
	private $userId = -1;
	private $adId   = -1;
	private $description = '';
	private $interestPublishingDate = NULL;
	
	public function __construct($aId, $aUserId, $aAdId, $aDescription, $aInterestPublishingDate) {
		$this->id          = $aId;
		$this->userId      = $aUserId;
		$this->adId        = $aAdId;
		$this->description            = $aDescription;
		$this->interestPublishingDate = $aInterestPublishingDate;
	}

	public function __destruct() {
		
	}

	public function __toString() {
		return "Interès amb id " . $this->id . " de l usuari ". $this->userId . ", amb adId " . $this->adId . ", descripció " . $this->description . " i data " . $this->interestPublishingDate;
	}

	public function getId() {
		return $this->id;
	}
	public function getUserId() {
		return $this->userId;
	}
	public function getAdId() {
		return $this->adId;
	}
	public function getDescription() {
		return $this->description;
	}
	public function getInterestPublishingDate() {
		return $this->interestPublishingDate;
	}
	

	/**
	 * Mostra la capçalera d'una taula d'anuncis d'altri
	 * @return string
	 */
	public static function getHtmlInterestsRowHeader() {
		if($GLOBALS['debug']) {
			$d_id="<td>id</td>";
			$d_userId="<td>userId</td>";
			$d_adId="<td>adId</td>";
		}
		return "<table border=\"1\">"
				. "<tr bgcolor=\"#D0D0D0\">"
				. $d_id
				. $d_userId
				. $d_adId
				. "<td>Descripci&oacute;</td>"
				. "<td>Data del missatge</td>"
				. "</tr>";
	}
	
	
	public static function getHtmlInterestsRowFooter() {
		return "</table>";
	}
	

	/**
	 * Mostra una fila d'una taula d'interesos en anunci 
	 * @return string
	 */
	public function toHtmlRow() {
		if($GLOBALS['debug']) {
			$d_id     = Interest::toHtmlCell($this->getId());
			$d_userId = Interest::toHtmlCell($this->getUserId());
			$d_adId   = Interest::toHtmlCell($this->getAdId());
		}
		return "<tr>"
				. $d_id
				. $d_userId
				. $d_adId
				. Interest::toHtmlCell($this->getDescription())
				. Interest::toHtmlCell($this->getInterestPublishingDate())
				. "</tr>";
	}
	
	private static function toHtmlCell($text) {
		return "<td bgcolor=\"#F2F2F2\">" . $text . "</td>";
	}
	
	
}

?>
