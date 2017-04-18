<?php

require_once("conf.inc");
require_once("class.user.inc.php");
require_once("class.grade.inc.php");
require_once("liberlib.inc");
require_once("dao.inc");
set_error_handler("myErrorHandler");

$cursId     = 1;
$userMail   = 'zoquero@gmail.com';
$userId     = 2;
$interestId = 1;

echo "Liber:  Algunes proves\n";
echo "======================\n";
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

$unUsuari = $dao->getUserByMail($userMail);
echo "Usuari (byMail): " . $unUsuari . "\n";

$unUsuari = $dao->getUserById($userId);
echo "Usuari (byId):   " . $unUsuari . "\n";

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
