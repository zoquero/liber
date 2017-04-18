<?php

class Ad {
	private $id     = -1;
	private $owner  = -1;
	private $isbn   = -1;
	private $status = -1;
	private $summary     = '';
	private $description = '';
	private $adPublishingDate = NULL;
	private $image = '';
	private $grade = -1;
	
	/**
	 * Array d'objectes Interest
	 * @var Array d'objectes Interest
	 * @see isSomeBodyInterested
	 */
	private $interests = array();
	
	public static $ROW_TO_DELETE        = 1;
	public static $ROW_TO_BE_INTERESTED = 2;
	
	public static $STATUS_PUBLISHED = 1;
	public static $STATUS_SOFT_DELETED = 2;
	
	public function __construct($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade) {
		$this->id    = $aId;
		$this->owner = $aOwner;
		$this->isbn  = $aIsbn;
		$this->status  = $aStatus;
		$this->summary = $aSummary;
		$this->description      = $aDescription;
		$this->adPublishingDate = $aAdPublishingDate;
		$this->image = $aImage;
		$this->grade = $aGrade;
	}

	public function __destruct() {
		
	}

	public function __toString() {
		return "Ad del usuari". $this->id . ", amb isbn " . $this->isbn . ", summary " . $this->summary . ", imatge " . $this->image . " del curs " . $this->aGrade;
	}
	
	/**
	 * Mostra una taula d'anuncis ($accio==$ROW_TO_DELETE), o bé els propis o bé els d'altri ($accio==$ROW_TO_BE_INTERESTED)
	 * @param int  $accio $ROW_TO_DELETE pels propis i $ROW_TO_BE_INTERESTED pels d'altri
	 * @return string
	 */
	public function toHtmlAnchoredRowDEPRECATED($accio) {
		$url="";
		if($accio == Ad::$ROW_TO_DELETE) {
			$d_accio = "<td bgcolor=\"#F2F2F2\">
						<form action=\"?\" method=\"post\" accept-charset=\"utf-8\">
							<input type=\"hidden\" name=\"actionId\" value=" . ACTION_DELETE_ADD . " />
							<input type=\"hidden\" name=\"id\" value=\"" . $this->getId() . "\"/>
							<input type=\"submit\" name=\"Esborra\" value=\"Esborra\" />
						</form></td>";
			$url= "?actionId=" . ACTION_DELETE_ADD . "&id=" . $this->getId();
		}
		else if($accio == Ad::$ROW_TO_BE_INTERESTED) {
			$d_accio = "<td bgcolor=\"#F2F2F2\">
						<form action=\"?\" method=\"post\" accept-charset=\"utf-8\">
							<input type=\"hidden\" name=\"actionId\" value=" . ACTION_BE_INTERESTED_IN_ADD . " />
							<input type=\"hidden\" name=\"id\" value=\"" . $this->getId() . "\"/>
							<input type=\"submit\" name=\"Interessat\" value=\"Hi tinc interès\" />
						</form></td>";
			$url= "?actionId=" . ACTION_BE_INTERESTED_IN_ADD . "&id=" . $this->getId();
		}
		else {
			trigger_error("Error de programació: Acció desconeguda sobre fila d'anunci", E_USER_ERROR);
		}
		
		if($GLOBALS['debug']) {
			$d_id=Ad::toHtmlAnchoredCol($this->getId(), $url, $accio);
			$d_owner=Ad::toHtmlAnchoredCol($this->getOwner(), $url, $accio);
			$d_status=Ad::toHtmlAnchoredCol($this->getStatus(), $url, $accio);
		}
		return
		      "<tr>"
		    . $d_accio
		    . $d_id
			. $d_owner
			. Ad::toHtmlAnchoredCol("PENDENT DE OMPLIR", $url, $accio)
			. Ad::toHtmlAnchoredCol($this->getIsbn(), $url, $accio)
			. $d_status
			. Ad::toHtmlAnchoredCol($this->getSummary(), $url, $accio)
			. Ad::toHtmlAnchoredCol($this->getDescription(), $url, $accio)
			. Ad::toHtmlAnchoredCol($this->getAdPublishingDate(), $url, $accio)
			. "</tr>";
	}
	
	/**
	 * Mostra una taula d'anuncis d'altri (no els propis)
	 * @param Grade $aGrade L'objecte curs al que està associat
	 * @return string
	 */	
	public function toHtmlAnchoredRowNotOwn($aGrade) {
		if($aGrade == NULL) {
			trigger_error("Error de programació: no s'ha rebut el curs associat a l'anunci", E_USER_WARNING);
		}
		$url="";
		$d_accio = "<td bgcolor=\"#F2F2F2\">
					<form action=\"?\" method=\"post\" accept-charset=\"utf-8\">
						<input type=\"hidden\" name=\"actionId\" value=" . ACTION_BE_INTERESTED_IN_ADD . " />
						<input type=\"hidden\" name=\"id\" value=\"" . $this->getId() . "\"/>
						<input type=\"submit\" name=\"Interessat\" value=\"Hi tinc interès\" />
					</form></td>";
		$url= "?actionId=" . ACTION_BE_INTERESTED_IN_ADD . "&id=" . $this->getId();
		
		if($GLOBALS['debug']) {
			$d_id=Ad::toHtmlAnchoredColNotOwn($this->getId(), $url);
			$d_owner=Ad::toHtmlAnchoredColNotOwn($this->getOwner(), $url);
			$d_status=Ad::toHtmlAnchoredColNotOwn($this->getStatus(), $url);
		}
		return
			"<tr>"
				. $d_accio
				. $d_id
				. $d_owner
				. Ad::toHtmlAnchoredColNotOwn($this->getIsbn(), $url)
				. $d_status
				. Ad::toHtmlAnchoredColNotOwn($this->getSummary(), $url)
				. Ad::toHtmlAnchoredColNotOwn($this->getDescription(), $url)
				. Ad::toHtmlAnchoredColNotOwn($this->getAdPublishingDate(), $url)
				. Ad::toHtmlAnchoredColNotOwn($aGrade->getName(), $url)
				. Ad::toHtmlAnchoredImgCol($this->getImage())
				. "</tr>";
	}
	
	
	

	/**
	 * Mostra una taula d'anuncis propis
	 * @return string
	 */
	public function toHtmlAnchoredRowOwn($aGrade) {
		$url="";
		$d_accio = "<td bgcolor=\"#F2F2F2\">
					<form action=\"?\" method=\"post\" accept-charset=\"utf-8\">
						<input type=\"hidden\" name=\"actionId\" value=" . ACTION_DELETE_ADD . " />
						<input type=\"hidden\" name=\"id\" value=\"" . $this->getId() . "\"/>
						<input type=\"submit\" name=\"Esborra\" value=\"Esborra\" />
					</form></td>";
		$url= "?actionId=" . ACTION_DELETE_ADD . "&id=" . $this->getId();
		
		$d_interests_html='';
		if($this->isSomeBodyInterested()) {
			$d_interests_html='<td bgcolor="#F5E2C2"><a href="?actionId=' . ACTION_SEE_NOTIFICATIONS . '&id=' . $this->getId() . '"><b>' . count($this->interests) . '</b> notificacions</a></td>';
		}
		else {
			$d_interests_html='<td bgcolor="#F2F2F2">0</td>';;
		}
		
		if($GLOBALS['debug']) {
			$d_id=Ad::toHtmlAnchoredColOwn($this->getId(), $url);
			$d_owner=Ad::toHtmlAnchoredColOwn($this->getOwner(), $url);
			$d_status=Ad::toHtmlAnchoredColOwn($this->getStatus(), $url);
		}
		return "<tr>"
				. $d_accio
				. $d_id
				. $d_owner
				. $d_interests_html
				. Ad::toHtmlAnchoredColOwn($this->getIsbn(), $url)
				. $d_status
				. Ad::toHtmlAnchoredColOwn($this->getSummary(), $url)
				. Ad::toHtmlAnchoredColOwn($this->getDescription(), $url)
				. Ad::toHtmlAnchoredColOwn($this->getAdPublishingDate(), $url)
				
				. Ad::toHtmlAnchoredColOwn($aGrade->getName(), $url)
				
				. Ad::toHtmlAnchoredImgCol($this->getImage())
				. "</tr>";
	}
	
	

	private static function toHtmlAnchoredColDEPRECATED($text, $url, $accio) {
		if($accio == Ad::$ROW_TO_BE_INTERESTED) {
			return "<td bgcolor=\"#F2F2F2\"><a href=\"" . $url . "\">" . $text . "</a></td>";
		}
		else {
			return "<td bgcolor=\"#F2F2F2\">" . $text . "</td>";
		}
	}

	private static function toHtmlAnchoredColOwn($text, $url) {
		return "<td bgcolor=\"#F2F2F2\">" . $text . "</td>";
	}
	
	
	
	private static function toHtmlAnchoredColNotOwn($text, $url) {
		return "<td bgcolor=\"#F2F2F2\"><a href=\"" . $url . "\">" . $text . "</a></td>";
	}


	private static function toHtmlAnchoredImgCol($text) {
		if(empty($text)) {
			return "<td bgcolor=\"#F2F2F2\">Sense imatge</td>";
		}
		else {
			$url=$GLOBALS['uploadFolderStore'] . "/" . $text;
			return "<td bgcolor=\"#F2F2F2\"><a target=\"_blank\" href=\"" . $url . "\"><img src=\"" . $GLOBALS['uploadFolderStore'] . "/" . $text . $GLOBALS['thumbFilenameSufix'] . "\"/></a></td>";
		}
	}
	
	
	/**
	 * Mostra la capçalera d'una taula d'anuncis ($accio==$ROW_TO_DELETE), o bé els propis o bé els d'altri ($accio==$ROW_TO_BE_INTERESTED)
	 * @param int  $accio $ROW_TO_DELETE pels propis i $ROW_TO_BE_INTERESTED pels d'altri
	 * @return string
	 */
	public static function getHtmlAdsRowHeaderDEPRECATED() {
		if($GLOBALS['debug']) {
			$d_id="<td>id</td>";
			$d_owner="<td>owner</td>";
			$d_status="<td>Status</td>";
		}
		return "<table border=\"1\">"
		    . "<tr bgcolor=\"#D0D0D0\">"
		    . "<td>Acci&oacute;</td>"
		    . $d_id
			. $d_owner
		    . "<td>Notificacions</td>"
			. "<td>ISBN</td>"
			. $d_status
			. "<td>Resum</td>"
			. "<td>Descripci&oacute;</td>"
			. "<td>Data d'anunci</td>"
			. "</tr>"; 
	}

	

	/**
	 * Mostra la capçalera d'una taula d'anuncis propis
	 * @return string
	 */
	public static function getHtmlAdsRowHeaderOwn() {
		if($GLOBALS['debug']) {
			$d_id="<td>id</td>";
			$d_owner="<td>owner</td>";
			$d_status="<td>Status</td>";
		}
		return "<table border=\"1\">"
				. "<tr bgcolor=\"#D0D0D0\">"
				. "<td>Acci&oacute;</td>"
				. $d_id
				. $d_owner
				. "<td>Notificacions</td>"
				. "<td>ISBN</td>"
				. $d_status
				. "<td>Resum</td>"
				. "<td>Descripci&oacute;</td>"
				. "<td>Data d'anunci</td>"
				. "<td>Curs</td>"
				. "<td>Imatge</td>"
				. "</tr>";
	}
	
	

	/**
	 * Mostra la capçalera d'una taula d'anuncis d'altri
	 * @return string
	 */
	public static function getHtmlAdsRowHeaderNotOwn() {
		if($GLOBALS['debug']) {
			$d_id="<td>id</td>";
			$d_owner="<td>owner</td>";
			$d_status="<td>Status</td>";
		}
		return "<table border=\"1\">"
				. "<tr bgcolor=\"#D0D0D0\">"
				. "<td>Acci&oacute;</td>"
				. $d_id
				. $d_owner
				. "<td>ISBN</td>"
				. $d_status
				. "<td>Resum</td>"
				. "<td>Descripci&oacute;</td>"
				. "<td>Data d'anunci</td>"
				. "<td>Curs</td>"
				. "<td>Imatge</td>"
				. "</tr>";
	}
	
	
	public static function getHtmlAdsRowFooter() {
		return "</table>";
	}
	

	public function getId() {
		return $this->id;
	}
	public function getOwner() {
		return $this->owner;
	}
	public function getIsbn() {
		return $this->isbn;
	}
	public function getStatus() {
		return $this->status;
	}
	public function getSummary() {
		return $this->summary;
	}
	public function getDescription() {
		return $this->description;
	}
	public function getAdPublishingDate() {
		return $this->adPublishingDate;
	}
	public function getImage() {
		return $this->image;
	}
	public function getGrade() {
		return $this->grade;
	}	
	public function setInterests($sInterests) {
		if(! is_array($sInterests)) {
			trigger_error("Compte, s'ha rebut a setInterests quelcom que no és un array", E_WARNING);
			return;
		}
		$this->interests = $sInterests;
	}
	
	public function getInterests() {
		return $this->interests;
	}
	
	public function isSomeBodyInterested() {
		return $this->interests != NULL && is_array($this->interests) && count($this->interests) > 0;
	}
	
}

?>