<?php

/*
 * SQL Command run:
 * mysql        -h localhost -u $liberdbuser -p$password -D liberdb -e "select * from ads"
 * 
 * DDL gen:
 * mysqldump -d -h localhost -u $liberdbuser -p$password liberdb
*/


define('ROOT_PATH', dirname(__FILE__) . '/../www/');

require_once(ROOT_PATH . "conf.inc");
require_once(ROOT_PATH . "class.user.inc.php");
require_once(ROOT_PATH . "liberlib.inc");
require_once(ROOT_PATH . "dao.inc");
require_once(ROOT_PATH . "class.grade.inc.php");

set_error_handler("myErrorHandler");

$cursId     = 1;
$userMail   = 'zoquero@gmail.com';
$userId     = 2;
$interestId = 1;

echo "============================\n";
echo "Proves de l'aplicació Liber:\n";
echo "============================\n";

# echo "Test get user (mocking):\n";
# $userHash = getUser_FileMock();
# print_r($userHash);

echo "Test get user (real):\n";
$userHash = getUser("0000-0000");
print_r($userHash);

$dao = new Dao;
if($dao->isErrConnecting()) {
	trigger_error("No ha estat possible connectar amb la base de dades", E_USER_ERROR);
	exit(1);
}
$unCurs = $dao->getGradeById($cursId);
echo "Curs:            [" . $unCurs->__toString() . "]\n";

$elsCursosHabilitats = $dao->getEnabledGrades();
echo "Tots els cursos:\n";
foreach($elsCursosHabilitats as $unCurs) {
	echo "\t" . $unCurs . "\n";
}

//
// Finalment no mantenim taula d'usuaris, creiem en SSO i au
//
// $unUsuari = $dao->getUserByMail($userMail);
// echo "Usuari (byMail): " . $unUsuari . "\n";
//
// $unUsuari = $dao->getUserById($userId);
// echo "Usuari (byId):   " . $unUsuari . "\n";
//

$unsInteressos = $dao->getInterestsByAdId($interestId);
if($unsInteressos == NULL || empty($unsInteressos)) {
	echo "Interessos (per l'anunci d'id $interestId): No se n'han trobat\n";
}
else {
	echo "Interessos (per l'anunci d'id $interestId):\n";
	foreach($unsInteressos as $unInteres) {
		echo "\t" . $unInteres . "\n";
	}
}

?>
