<?php

require_once("conf.inc");
require_once("class.ad.inc.php");
require_once("class.grade.inc.php");
require_once("class.user.inc.php");
require_once("class.interest.inc.php");
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
			$this->mysqliCon->close();
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

/** 
	public function getCurrentUser() {
		$u = ''; // PENDENT de veure com agafar-ho de la sessió FJE
		if($GLOBALS['mock']) {
			if(! empty($_SESSION["dummyUsername"])) {
				$u = $_SESSION["dummyUsername"];
			}
			else {
				trigger_error("Error de programa: Sempre ha d'haver una sessi&oacute; d'usuari", E_USER_WARNING);
				return NULL;
			}
		}
		if($u === '') {
			if($GLOBALS['mock']) {
				trigger_error("Error d'aplicaci&oacute;: Sempre ha d'haver una sessi&oacute; d'usuari", E_USER_WARNING);
				return NULL;
			}
			else {
				trigger_error("Error d'aplicaci&oacute;: Sempre ha d'haver una sessi&oacute; d'usuari", E_USER_WARNING);
				return NULL;
			}
		}
		return $this->getUserByMail($u);
	}
 */

/*

	 **
	 * Retorna l'objecte User a partir de l'entrada a taula d'usuaris amb tal mail
	 * @param String $m El mail
	 * @return User|NULL L'objecte User o NULL si no es troba.
	 *
	public function getUserByMail($m) {
		if($m == NULL || $m == ''){
			trigger_error("Error de programació: Rebut mail NULL fent getUserByMail", E_USER_WARNING);
			return NULL;
		}
		$aId='';
		$aMail='';
		$aFullname='';
		// La consulta
		$sql = "SELECT id, mail, fullname FROM members where mail = ?";

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			$stmt->bind_param("s", $m);
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aMail, $aFullname);
			// llegim la resposta
			if ($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				return new User($aId, $aMail, $aFullname);
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return NULL;
	}

	 **
	 * Retorna l'objecte User a partir de l'entrada a taula d'usuaris amb tal id
	 * @param String $i L'id de l'usuari
	 * @return User|NULL L'objecte User o NULL si no es troba.
	 * 
	public function getUserById($i) {
		$aId='';
		$aMail='';
		$aFullname='';
		// La consulta
		$sql = "SELECT id, mail, fullname FROM members where id = ?";

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			if($stmt->bind_param("i", $i) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return NULL;
			}
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
				
			// associem resposta a variables
			$stmt->bind_result($aId, $aMail, $aFullname);
			// llegim la resposta
			if ($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				return new User($aId, $aMail, $aFullname);
			}
			if (! $stmt->close()) {
				trigger_error("Error tancant l'statement de la Base de Dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
		}
		else {
			trigger_error("No s'ha pogut preparar l'acció sobre la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			return NULL;
		}
		return NULL;
	}

*/

	public function createAd($user, $fIsbn, $fSummary, $fDescription, $fImage, $grade) {
		if($user == NULL){
			trigger_error("Error de programació: Rebut usuari NULL o buit creant un anunci", E_USER_WARNING);
			return FALSE;
		}
		if($grade == NULL){
			trigger_error("Error de programació: Rebut curs NULL o buit creant un anunci", E_USER_WARNING);
			return FALSE;
		}
		if($fIsbn == NULL || $fIsbn == '' || $fSummary == NULL || $fSummary == ''){
			trigger_error("Error de programació: Rebuts ISBN o Resum null o buit creant un anunci", E_USER_WARNING);
			return FALSE;
		}
		// La consulta
		$sql = "INSERT INTO ads (id, owner, isbn, status, summary, description, ad_publishing_date, image, grade) VALUES (NULL, ?, ?, ?, ?, ?, NOW(), ?, ?);";
//echo "dao.createAd: Anem a executar sql= " . $sql . "<br>\n";
		
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
			if($stmt->bind_param("iiisssi", $this->mysqliCon->real_escape_string($user->getId()), $this->mysqliCon->real_escape_string($fIsbn), $this->mysqliCon->real_escape_string(AD_STATUS_PUBLISHED), $this->mysqliCon->real_escape_string($fSummary), $this->mysqliCon->real_escape_string($fDescription), $this->mysqliCon->real_escape_string($fImage), $this->mysqliCon->real_escape_string($grade->getId())) === FALSE) {
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
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade FROM ads where owner = ?";

		/* Per aconseguir també mail i nom de l'anunciant:
		 *
		* SELECT ads.id, ads.owner, ads.isbn, ads.status,
		* 		ads.summary, ads.description, ads.ad_publishing_date, members.id,
		* 		members.mail, members.fullname FROM ads
		* 		INNER JOIN members on ads.owner = 1 and ads.owner = members.id
		*/
		 

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
				
			if($stmt->bind_param("i", $this->mysqliCon->real_escape_string($user->getId())) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return NULL;
			}
				
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade));
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
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade FROM ads where grade = ?";
	
		/* Per aconseguir també mail i nom de l'anunciant:
		 *
		* SELECT ads.id, ads.owner, ads.isbn, ads.status,
		* 		ads.summary, ads.description, ads.ad_publishing_date, members.id,
		* 		members.mail, members.fullname FROM ads
		* 		INNER JOIN members on ads.owner = 1 and ads.owner = members.id
		*/
			
	
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
	
			if($stmt->bind_param("i", $this->mysqliCon->real_escape_string($grade->getId())) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return NULL;
			}
	
			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade));
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
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade FROM ads where id = ?";

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {

			if($stmt->bind_param("i", $this->mysqliCon->real_escape_string($id)) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades:<br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
			}

			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade);
			// llegim la resposta
			if($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				$r = new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade);
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
			if($stmt->bind_param("ii", $this->mysqliCon->real_escape_string($rId), $this->mysqliCon->real_escape_string($rOwner)) === FALSE) {
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
		// La consulta
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade FROM ads where isbn = ?";

		/* Per aconseguir també mail i nom de l'anunciant:
		 *
		* SELECT ads.id, ads.owner, ads.isbn, ads.status,
		* 		ads.summary, ads.description, ads.ad_publishing_date, members.id,
		* 		members.mail, members.fullname FROM ads
		* 		INNER JOIN members on ads.owner = 1 and ads.owner = members.id
		*/
			

		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {

			if($stmt->bind_param("i", $this->mysqliCon->real_escape_string($pIsbn)) === FALSE) {
				trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
				return NULL;
			}

			// executem la consulta
			if( $stmt->execute() === FALSE) {
				trigger_error("No s'ha pogut executar la consulta a la base de dades: <br/>(" .  $this->mysqliCon->errno . ") " . $this->mysqliCon->error, E_USER_WARNING);
				return NULL;
			}
			// associem resposta a variables
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade);
			// llegim la resposta
			while($stmt->fetch()) {
				//				echo '<table><tr><td>'.$aId.'</td><td>'.$aMail.'</td><td>'.$aFullname.'</td></tr></table>';
				array_push($r, new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade));
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


	/**
	 * Creació d'un interès sobre un anunci
	 * @param User $user Objecte associat a l'usuari que està interessat
	 * @param Ad $theAd objecte Ad amb l'anunci sobre el que hi ha interès
	 * @param String $fDescription Descripció associada a l'interès
	 * @return boolean|NULL True si ok, false o NULL si error
	 */
	public function createInterest($user, $theAd, $fDescription) {
// echo "dao.createAd: rebem user id = " . $user->getId() . " ad id = " . $theAd->getId() . "<br/>\n";
		if($user == NULL){
			trigger_error("Error de programació: Rebut usuari NULL fent createInterest", E_USER_WARNING);
		}
		// La consulta
		$sql = "INSERT INTO interests (ad, interested, description, date) VALUES (?, ?, ?, NOW());";
// echo "dao.createAd: Anem a executar sql= " . $sql . "<br/>\n";
	
		// utilitzem un prepared statement
		if($stmt = $this->mysqliCon->prepare($sql)) {
// echo "dao.createAd: Fem stmt->bind_param amb usuari adId = " . $this->mysqliCon->real_escape_string($theAd->getId()) . " userId = ". $this->mysqliCon->real_escape_string($user->getId()) . " i descripció = " . $this->mysqliCon->real_escape_string($fDescription) . "<br/>";
			if($stmt->bind_param("iis", $this->mysqliCon->real_escape_string($theAd->getId()), $this->mysqliCon->real_escape_string($user->getId()), $this->mysqliCon->real_escape_string($fDescription)) === FALSE) {
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
		
		// La consulta
		/*
		select * from ads where MATCH (`summary`, `description`) AGAINST ('mates');
		select * from ads where MATCH (`summary`, `description`) AGAINST ('Mate')
		 */
		
		$sql = "SELECT id, owner, isbn, status, summary, description, ad_publishing_date, image, grade FROM ads where MATCH (`summary`, `description`) AGAINST ( ? )";
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
					if($stmt->bind_param("s", $this->mysqliCon->real_escape_string($m[0])) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
				case 2:
					if($stmt->bind_param("ss", $this->mysqliCon->real_escape_string($m[0]), $this->mysqliCon->real_escape_string($m[1])) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
				case 3:
					if($stmt->bind_param("sss", $this->mysqliCon->real_escape_string($m[0]), $this->mysqliCon->real_escape_string($m[1]), $this->mysqliCon->real_escape_string($m[2])) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
				case 4:
					if($stmt->bind_param("ssss", $this->mysqliCon->real_escape_string($m[0]), $this->mysqliCon->real_escape_string($m[1]), $this->mysqliCon->real_escape_string($m[2]), $this->mysqliCon->real_escape_string($m[3])) === FALSE) {
						trigger_error("Errors associant variables a la consulta a la base de dades", E_USER_WARNING);
						return NULL;
					}
					break;
				case 5:
					if($stmt->bind_param("sssss", $this->mysqliCon->real_escape_string($m[0]), $this->mysqliCon->real_escape_string($m[1]), $this->mysqliCon->real_escape_string($m[2]), $this->mysqliCon->real_escape_string($m[3]), $this->mysqliCon->real_escape_string($m[4])) === FALSE) {
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
			$stmt->bind_result($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade);
			// llegim la resposta
			while($stmt->fetch()) {
				$anAdRead=new Ad($aId, $aOwner, $aIsbn, $aStatus, $aSummary, $aDescription, $aAdPublishingDate, $aImage, $aGrade);
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
		$sql = "SELECT id, name, enabled FROM grades where enabled = TRUE";
	
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
	
}
?>
