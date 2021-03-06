<?php

require_once("conf.inc");
require_once("class.ad.inc.php");
require_once("class.grade.inc.php");
// require_once("class.user.inc.php"); // no user table
require_once("class.interest.inc.php");
require_once("class.logentry.inc.php");
require_once("class.notifyme.inc.php");
require_once("liberlib.inc");


class Dao {

	private $mysqliCon = NULL;
	private $errConnecting = FALSE;

	private function connect() {
		$this->mysqliCon = new mysqli($GLOBALS['dbHostname'], $GLOBALS['dbUsername'], $GLOBALS['dbPassword'], $GLOBALS['dbName']);
		$ret = $this->mysqliCon->connect_errno;
		if($ret != 0) {
			printf("Error connectant contra la base dades: (%s) %s\n", $ret, $this->mysqliCon->connect_error);
			return FALSE;
		}
		return TRUE;
	}

	private function disconnect() {
		if($this->mysqliCon != NULL && ! $this->mysqliCon) {
			if($this->mysqliCon->close()) {
				$this->mysqliCon = null;
			}
		}
	}


	/**
	 * Posem el charset a UTF-8
	 * @return boolean True si correcte, false si errors.
	 */
	private function setCharset() {
		if (!$this->mysqliCon->set_charset("utf8")) {
			trigger_error("Error carregant el charset utf8: (" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		else {
			return TRUE;
		}
	}

	public function __construct() {
		if(! $this->connect()) {
			$this->errConnecting=TRUE;
		}
		else {
			if(! $this->setCharset()) {
				$this->errConnecting=TRUE;
			}
		}
		$this->errConnecting=FALSE;
	}

	public function __destruct() {
		$this->disconnect();
	}

	public function __toString() {
		return "Dao";
	}

	public function isErrConnecting() {
		return $this->errConnecting;
	}
	

	public function createAd($user, $fIsbn, $fSummary, $fDescription, $fImage, $grade, $notified = FALSE) {
		if($user == NULL){
			trigger_error("Error de programació: Rebut usuari NULL o buit creant un anunci", E_USER_WARNING);
			return FALSE;
		}
		if($grade == NULL){
			trigger_error("Error de programació: Rebut curs NULL o buit creant un anunci", E_USER_WARNING);
			return FALSE;
		}
		// Finalment no exigim el ISBN
		/*
		 *  if($fIsbn == NULL || $fIsbn == '' || $fSummary == NULL || $fSummary == ''){
		 *	 trigger_error("Error de programació: Rebuts ISBN o Resum null o buit creant un anunci", E_USER_WARNING);
		 *	 return FALSE;
		 *  }
		 */
		 
		// La consulta
		$sql = "INSERT INTO ads (id, owner, isbn, status, summary, description, ad_publishing_date, image, grade, notified) VALUES (NULL, ?, ?, ?, ?, ?, NOW(), ?, ?, ?);";
//echo "dao.createAd: Anem a executar sql= " . $sql . "<br>\n";
		
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			$scapedMail            = $this->mysqliCon->real_escape_string($user['mail']);
			$scapedFIsbn           = $this->mysqliCon->real_escape_string($fIsbn);
			$scapedStatusPublished = $this->mysqliCon->real_escape_string(AD_STATUS_PUBLISHED);
			$scapedSummary         = $this->mysqliCon->real_escape_string($fSummary);
			$scapedFDescription    = $this->mysqliCon->real_escape_string($fDescription);
			$scapedImage           = $this->mysqliCon->real_escape_string($fImage);
			$scapedGradeId         = $this->mysqliCon->real_escape_string($grade->getId());
			$scapedNotified        = boolval($notified);
			if($stmt->bind_param("siisssii", $scapedMail, $scapedFIsbn, $scapedStatusPublished, $scapedSummary, $scapedFDescription, $scapedImage, $scapedGradeId, $scapedNotified) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
				
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la inserció a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			if ($this->mysqliCon->affected_rows != 1) {
				trigger_error("No s'ha pogut inserir l'anunci a la Base de Dades", E_USER_WARNING);
				return FALSE;
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			// echo "dao.createAd: Fet<br>\n";
			return TRUE;
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		return FALSE;
	}

	/**
	 * Retorna l'array d'objectes Ad de l'usuari representat per l'objecte rebut user
	 * @param unknown $user objecte user rebut representant a l'usuari
	 * @return array d'objectes Ad de l'usuari representat per l'objecte rebut user
	 */
	function getAdsByAdvertiser($user) {
		if($user == NULL){
			trigger_error("Error de programació: Rebut usuari NULL fent getAdsByAdvertiser", E_USER_WARNING);
			return NULL;
		}
		$r = array(); // Ad
		$aId='';
		$aOwner='';
		$aIsbn='';
		$aStatus='';
		$aSummary='';
		$aDescription='';
		$aAdPublishingDate='';
		$aImage='';
		$aGrade=-1;
		$aNotified=FALSE;
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade, notified FROM ads where owner = ?";

		/*
		 * Comentari legacy, de quan hi havia una taula d'usuaris:
		 *
		 * Per aconseguir també mail i nom de l'anunciant:
		 *
		 * SELECT ads.id, ads.owner, ads.isbn, ads.status,
		 * 		ads.summary, ads.description, ads.ad_publishing_date, members.id,
		 * 		members.mail, members.fullname FROM ads
		 * 		INNER JOIN members on ads.owner = 1 and ads.owner = members.id
		 */
		 

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
		
			$scaped = $this->mysqliCon->real_escape_string($user['mail']);
			if($stmt->bind_param("s", $scaped) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return NULL;
			}
				
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}
	
	
	/**
	 * Retorna l'array d'objectes Ad dels anuncis encara no notificats.
	 * @return array d'objectes Ad encara no notificats
	 */
	function getUnnotifiedAds() {
		$r = array(); // Ad
		$aId='';
		$aOwner='';
		$aIsbn='';
		$aStatus='';
		$aSummary='';
		$aDescription='';
		$aAdPublishingDate='';
		$aImage='';
		$aGrade=-1;
		$aNotified=FALSE;
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade, notified FROM ads where notified = FALSE and status = 1";

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}
	
	
	/**
	 * Retorna l'array d'objectes Ad de tal curs
	 * @param Grade $user objecte grade representant tal curs
	 * @return array d'objectes Ad de tal curs
	 */
	function getAdsByGrade($grade) {
		if($grade == NULL){
			trigger_error("Error de programació: Rebut curs NULL fent getAdsByGrade", E_USER_WARNING);
			return NULL;
		}
		$r = array(); // Ad
		$aId='';
		$aOwner='';
		$aIsbn='';
		$aStatus='';
		$aSummary='';
		$aDescription='';
		$aAdPublishingDate='';
		$aImage='';
		$aGrade=-1;
		$aNotified=FALSE;
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade, notified FROM ads where grade = ?";
	
		/*
		 * Comentari legacy, de quan hi havia una taula d'usuaris:
		 * 
		 * Per aconseguir també mail i nom de l'anunciant:
		 *
		 * SELECT ads.id, ads.owner, ads.isbn, ads.status,
		 * 		ads.summary, ads.description, ads.ad_publishing_date, members.id,
		 * 		members.mail, members.fullname FROM ads
		 * 		INNER JOIN members on ads.owner = 1 and ads.owner = members.id
		 */
			
	
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
	
			$scapedId = $this->mysqliCon->real_escape_string($grade->getId());
			if($stmt->bind_param("i", $scapedId) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return NULL;
			}
	
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}
	
	
	/**
	 * Retorna l'array de tots els objectes Ad
	 * @return array d'objectes Ad
	 */
	function getAllAds() {
		$r = array(); // Ad
		$aId='';
		$aOwner='';
		$aIsbn='';
		$aStatus='';
		$aSummary='';
		$aDescription='';
		$aAdPublishingDate='';
		$aImage='';
		$aGrade=-1;
		$aNotified=FALSE;
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade, notified FROM ads";
	
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}
	
	
	function getAdById($id) {
		$r=NULL;
		$aId='';
		$aOwner='';
		$aIsbn='';
		$aStatus='';
		$aSummary='';
		$aDescription='';
		$aAdPublishingDate='';
		$aImage='';
		$aGrade=-1;
		$aNotified=FALSE;
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade, notified FROM ads where id = ?";

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {

			$scaped = $this->mysqliCon->real_escape_string($id);
			if($stmt->bind_param("i", $scaped) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			}

			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
			// llegim la resposta
			if($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				$r = new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}



	function deleteAdById($rId, $rOwner) {
		// La consulta
		$sql = "UPDATE ads set status = ". Ad::$STATUS_SOFT_DELETED . " where id = ? and owner = ?";

		// echo "<p>Executarem sql = " . $sql . "</p>";
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			$scapedRId = $this->mysqliCon->real_escape_string($rId);
			$scapedROwner = $this->mysqliCon->real_escape_string($rOwner['mail']  );
			if($stmt->bind_param("is", $scapedRId, $scapedROwner) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return FALSE;
			}

			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar l'esborrat a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			if ($this->mysqliCon->affected_rows != 1) {
				trigger_error("No s'ha pogut inserir l'anunci a la Base de Dades", E_USER_WARNING);
				return FALSE;
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			// echo "dao.createAd: Fet<br>\n";
			return TRUE;
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		return FALSE;
	}


	function getAdsByIsbn($pIsbn) {
		$r = array(); // Ad
		$aId='';
		$aOwner='';
		$aIsbn='';
		$aStatus='';
		$aSummary='';
		$aDescription='';
		$aAdPublishingDate='';
		$aImage='';
		$aGrade=-1;
		$aNotified=FALSE;
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade, notified FROM ads where isbn = ?";

		/*
		 * Comentari legacy, de quan hi havia una taula d'usuaris:
		 *
		 * Per aconseguir també mail i nom de l'anunciant:
		 *
		 * SELECT ads.id, ads.owner, ads.isbn, ads.status,
		 * 		ads.summary, ads.description, ads.ad_publishing_date, members.id,
		 * 		members.mail, members.fullname FROM ads
		 * 		INNER JOIN members on ads.owner = 1 and ads.owner = members.id
		 */
			

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {

			$scaped = $this->mysqliCon->real_escape_string($pIsbn);
			if($stmt->bind_param("i", $scaped) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return NULL;
			}

			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}

	public function updateAdSetNotified($adId) {
		if($adId == NULL || $adId < 0){
			trigger_error("Error de programació: Rebut adId NULL o erroni a updateAdSetNotified", E_USER_WARNING);
			return FALSE;
		}
		// La consulta
		$sql = "UPDATE ads set notified = TRUE where id = ?;";
//echo "dao.updateAdSetNotified: Anem a executar sql= " . $sql . "<br>\n";
		
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			$scapedId = intval($adId);
			if($stmt->bind_param("i", $scapedId) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
				
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la inserció a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			if ($this->mysqliCon->affected_rows != 1) {
				trigger_error("No s'ha pogut actualitzar l'anunci a la Base de Dades", E_USER_WARNING);
				return FALSE;
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			// echo "dao.updateAdSetNotified: Fet<br>\n";
			return TRUE;
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		return FALSE;
	}


	/**
	 * Creació d'un interès sobre un anunci
	 * @param User $user Objecte associat a l'usuari que està interessat (has tret del json)
	 * @param Ad $theAd objecte Ad amb l'anunci sobre el que hi ha interès
	 * @param String $fDescription Descripció associada a l'interès
	 * @return boolean|NULL True si ok, false o NULL si error
	 */
	public function createInterest($user, $theAd, $fDescription) {
// echo "dao.createAd: rebem user mail = " . $user['mail'] . " ad id = " . $theAd->getId() . "<br/>\n";
		if($user == NULL){
			trigger_error("Error de programació: Rebut usuari NULL fent createInterest", E_USER_WARNING);
		}
		// La consulta
		$sql = "INSERT INTO interests (ad, interested, description, date) VALUES (?, ?, ?, NOW());";
// echo "dao.createAd: Anem a executar sql= " . $sql . "<br/>\n";
	
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
// echo "dao.createAd: Fem stmt->bind_param amb usuari adId = " . $this->mysqliCon->real_escape_string($theAd->getId()) . " userId = ". $this->mysqliCon->real_escape_string($user->getId()) . " i descripció = " . $this->mysqliCon->real_escape_string($fDescription) . "<br/>";
			$scapedId          = $this->mysqliCon->real_escape_string($theAd->getId());
			$scapedMail        = $this->mysqliCon->real_escape_string($user['mail']);
			$scapedDescription = $this->mysqliCon->real_escape_string($fDescription);
			if($stmt->bind_param("iss", $scapedId, $scapedMail, $scapedDescription) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
	
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la inserció a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			if ($this->mysqliCon->affected_rows != 1) {
				trigger_error("No s'ha pogut inserir l'interès a la Base de Dades", E_USER_WARNING);
				return NULL;
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// echo "dao.createAd: Fet<br>\n";
			return TRUE;
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return NULL;
	}
	
	/**
	 * Retorna l'array d'objectes Interest associats a un Ad
	 * @param String $m El Ad->id
	 * @return array::Interest|NULL L'array d'objectes Interest o NULL si errors
	 */
	public function getInterestsByAdId($m) {
		$r=array();
		if($m == NULL || $m == ''){
			trigger_error("Error de programació: Rebut id NULL fent getInterestsByAdId", E_USER_WARNING);
			return NULL;
		}
		$aId=-1;
		$aInterested=-1;
		$aAdId=-1;
		$aDescription='';
		$aDate=NULL;
		// La consulta
		$sql = "SELECT id, ad, interested, description, date FROM interests WHERE ad = ?";
// echo "Buscarem interests sobre l'ad amb id = $m amb sql = $sql<br/>";
		
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			
			if($stmt->bind_param("i", $m) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aInterested, $aAdId, $aDescription, $aDate);
			// llegim la resposta
			while($stmt->fetch()) {
				array_push($r, new Interest($aId, $aInterested, $aAdId, $aDescription, $aDate));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("Errors preparant la consulta sobre la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}
	

	/**
	 * Retorna l'array d'objectes Interest resultants de cercar per paraular clau
	 * @param array(String) $m L'array de paraules clau
	 * @return array::Interest|NULL L'array d'objectes Interest o NULL si errors
	 * @see http://www.petefreitag.com/item/477.cfm
	 */
	public function searchAdsByKeywords($m) {
		if(! is_array($m) ) {
			trigger_error("Error de programació: searchAdsByKeywords: Rebut com a paràmetre variable no array", E_USER_WARNING);
			return NULL;
		}
		if(! count($m) > 0) {
			trigger_error("Error a searchAdsByKeywords: No s'ha rebut un array no buit", E_USER_WARNING);
			return NULL;
		}
		if(count($m) > 5) {
			trigger_error("Error a searchAdsByKeywords: No poden buscar-se més de 5 paraules clau en cada cerca", E_USER_WARNING);
			return NULL;
		}
		if($m == NULL || $m == ''){
			trigger_error("Error de programació: Rebut id NULL fent getInterestsByAdId", E_USER_WARNING);
			return NULL;
		}
		
		$r=array();
		$aId=-1;
		$aOwner=-1;
		$aIsbn=-1;
		$aStatus=-1;
		$aSummary='';
		$aDescription='';
		$aAdPublishingDate=NULL;
		$aImage='';
		$aGrade=-1;
		$aNotified=FALSE;
		
		// La consulta
		/*
		select * from ads where MATCH (`summary`, `description`) AGAINST ('mates');
		select * from ads where MATCH (`summary`, `description`) AGAINST ('Mate')
		 */
		
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade, notified FROM ads where MATCH (`summary`, `description`) AGAINST ( ? )";
		if(count($m) > 1) {
			for($i = 1; $i < count($m); $i++) {
				$sql .= " OR MATCH (`summary`, `description`) AGAINST ( ? )";
			}
		}
// echo "utilitzarem sql=<b>" . $sql . "<br/>count = " . count($m) . "</b><br/>\n";
	
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			switch(count($m)) {
				case 1:
					$scaped0 = $this->mysqliCon->real_escape_string($m[0]);
					if($stmt->bind_param("s", $scaped0) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
				case 2:
					$scaped0 = $this->mysqliCon->real_escape_string($m[0]);
					$scaped1 = $this->mysqliCon->real_escape_string($m[1]);
					if($stmt->bind_param("ss", $scaped0, $scaped1) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
				case 3:
					$scaped0 = $this->mysqliCon->real_escape_string($m[0]);
					$scaped1 = $this->mysqliCon->real_escape_string($m[1]);
					$scaped2 = $this->mysqliCon->real_escape_string($m[2]);
					if($stmt->bind_param("sss", $scaped0, $scaped1, $scaped2) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
				case 4:
					$scaped0 = $this->mysqliCon->real_escape_string($m[0]);
					$scaped1 = $this->mysqliCon->real_escape_string($m[1]);
					$scaped2 = $this->mysqliCon->real_escape_string($m[2]);
					$scaped3 = $this->mysqliCon->real_escape_string($m[3]);
					if($stmt->bind_param("ssss", $scaped0, $scaped1, $scaped2, $scaped3) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
				case 5:
					$scaped0 = $this->mysqliCon->real_escape_string($m[0]);
					$scaped1 = $this->mysqliCon->real_escape_string($m[1]);
					$scaped2 = $this->mysqliCon->real_escape_string($m[2]);
					$scaped3 = $this->mysqliCon->real_escape_string($m[3]);
					$scaped4 = $this->mysqliCon->real_escape_string($m[4]);
					if($stmt->bind_param("sssss", $scaped0, $scaped1, $scaped2, $scaped3, $scaped4) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
			}
			
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
			// llegim la resposta
			while($stmt->fetch()) {
				$anAdRead=new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade, $aNotified);
				array_push($r, $anAdRead);
			}

			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}

	
	/**
	 * Retorna l'objecte Grade a partir de l'entrada a taula de cursos amb tal id
	 * @param int $i L'id del curs
	 * @return User|NULL L'objecte Grade o NULL si no es troba o hi ha errors
	 */
	public function getGradeById($i) {
		$aId='';
		$aName='';
		$aEnabled=FALSE;
		// La consulta
		$sql = "SELECT id, name, enabled FROM grades where id = ?";
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			if($stmt->bind_param("i", $i) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
	
			// associem resposta a variables
			$stmt->bind_result($aId, $aName, $aEnabled);
			// llegim la resposta
			if ($stmt->fetch()) {
				return new Grade($aId, $aName, $aEnabled);
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return NULL;
	}
	
	

	/**
	 * Retorna un array amb els objectes Grade representant els cursos o NULL si errors o no n'hi ha
	 * @return NULL|multitype: array d'objectes Grade o NULL si errors o no n'hi ha
	 */
	function getEnabledGrades() {
		$r = array(); // Ad
		$aId='';
		$aName='';
		$aEnabled=FALSE;
		// La consulta
		$sql = "SELECT id, name, enabled FROM grades where enabled = TRUE ORDER BY id";
	
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
	
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aName, $aEnabled);
			// llegim la resposta
			while($stmt->fetch()) {
				array_push($r, new Grade($aId, $aName, $aEnabled));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}

	public function insertLogEntry($actionId, $user, $description) {
		if($user == NULL){
//			trigger_error("Error de programació: usuari NULL: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		if($actionId == NULL){
			$actionId = ACTION_NONE;
		}
		if($description == NULL){
//			trigger_error("Error de programació: description NULL: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		// La consulta
		$sql = "INSERT INTO logs (action, user, description) VALUES (?, ?, ?);";
//echo "dao.insertLog: Anem a executar sql= " . $sql . "<br>\n";
		
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			$scapedAction      = $this->mysqliCon->real_escape_string($actionId);
			$scapedMail        = $this->mysqliCon->real_escape_string($user['mail']);
			$scapedDescription = $this->mysqliCon->real_escape_string($description);
			if($stmt->bind_param("iss", $scapedAction, $scapedMail, $scapedDescription) === FALSE) {
//				trigger_error("Errors associant variables a la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}

			// executem la consulta
			if( $stmt->execute() === FALSE) {
//				trigger_error("No s'ha pogut executar la inserció a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			if ($this->mysqliCon->affected_rows != 1) {
//				trigger_error("No s'ha pogut inserir el log a la Base de Dades", E_USER_WARNING);
				return FALSE;
			}
			if (! $stmt->close()) {
//				trigger_error("Error tancant l'statement de la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
// echo "dao.insertLog: Fet<br>\n";
			return TRUE;
		}
		else {
//			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		return FALSE;
	}
	
	
	/**
	 * Retorna un array amb els objectes LogEntry les entrades de log llegides
	 * @return NULL|multitype: array d'objectes LogEntry o NULL si errors o no n'hi ha
	 */
	function getLogEntries() {
		$r = array(); // Ad
		$aTimestamp='';
		$aAction='';
		$aUser='';
		$aDescription='';
		// La consulta
		$sql = "SELECT timestamp, action, user, description FROM logs";
	
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
	
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aTimestamp, $aAction, $aUser, $aDescription);
			// llegim la resposta
			while($stmt->fetch()) {
				array_push($r, new LogEntry($aTimestamp, $aAction, $aUser, $aDescription));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}
	

	public function clearNotifymes($user) {
		if($user == NULL){
			trigger_error("Error de programació: Rebut usuari NULL o buit creant un Notifyme", E_USER_WARNING);
			return FALSE;
		}
		// La consulta
		$sql = "DELETE FROM notifymes WHERE owner = ?;";
//echo "dao.clearNotifymes: Anem a executar sql= " . $sql . "<br>\n";
		
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			$scapedMail            = $this->mysqliCon->real_escape_string($user['mail']);
			if($stmt->bind_param("s", $scapedMail) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
				
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar l'esborrat de notifymes a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			// echo "dao.createAd: Fet<br>\n";
			return TRUE;
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		return FALSE;
	}
	

	public function createNotifyme($user, $gradeId) {
		if($user == NULL){
			trigger_error("Error de programació: Rebut usuari NULL o buit creant un Notifyme", E_USER_WARNING);
			return FALSE;
		}
		if($gradeId == NULL){
			trigger_error("Error de programació: Rebut curs NULL o buit creant un Notifyme", E_USER_WARNING);
			return FALSE;
		}
		// La consulta
		$sql = "INSERT INTO notifymes (owner, grade) VALUES (?, ?);";
//echo "dao.createNotifyme: Anem a executar sql= " . $sql . "<br>\n";
		
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			$scapedMail            = $this->mysqliCon->real_escape_string($user['mail']);
			$scapedGradeId         = intval($gradeId);
			if($stmt->bind_param("si", $scapedMail, $scapedGradeId) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
				
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la inserció a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			if ($this->mysqliCon->affected_rows != 1) {
				trigger_error("No s'ha pogut inserir el notifyme a la Base de Dades", E_USER_WARNING);
				return FALSE;
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return FALSE;
			}
			// echo "dao.createAd: Fet<br>\n";
			return TRUE;
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return FALSE;
		}
		return FALSE;
	}
	
	/**
	 * Retorna l'array d'objectes Notifyme de l'usuari representat per l'objecte rebut user
	 * @param unknown $user objecte user rebut representant a l'usuari
	 * @return array d'objectes Notifyme de l'usuari representat per l'objecte rebut user
	 */
	function getNotifymesByOwner($user) {
		if($user == NULL){
			trigger_error("Error de programació: Rebut usuari NULL fent getNotifymesByOwner", E_USER_WARNING);
			return NULL;
		}
		$r = array(); // NotifyMe
		$aId='';
		$aOwner='';
		$aGrade=-1;
		// La consulta
		$sql = "SELECT id, owner, grade FROM notifymes where owner = ?";

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
		
			$scaped = $this->mysqliCon->real_escape_string($user['mail']);
			if($stmt->bind_param("s", $scaped) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return NULL;
			}
				
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aGrade);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new NotifyMe($aId, $aOwner, $aGrade));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}

	/**
	 * Retorna l'array d'objectes Notifyme (tots)
	 * @return array d'objectes Notifyme
	 */
	function getNotifymes() {
		$r = array(); // NotifyMe
		$aId='';
		$aOwner='';
		$aGrade=-1;
		// La consulta
		$sql = "SELECT id, owner, grade FROM notifymes";

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
		
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aGrade);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new NotifyMe($aId, $aOwner, $aGrade));
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return $r;
	}
}


?>
