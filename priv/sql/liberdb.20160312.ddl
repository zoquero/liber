utf8_unicode_ci ==> més universal i més lent
utf8_general_ci ==> menys universal però més ràpid
...
utf8mb4_general_ci    !!!!!!!!!!!!!! <<<<<<<<<<<<<<<<<<<<<<<

 ("SET NAMES 'utf8'")

$mysqli->set_charset("utf8"); ?


CREATE TABLE IF NOT EXISTS `table` (
`id` int(11) NOT NULL AUTO_INCREMENT,
`name` varchar(45) CHARACTER SET utf8 COLLATE utf8_spanish_ci NOT NULL,
`autotimestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


ALTER TABLE `ads` ADD CONSTRAINT `memberfk` FOREIGN KEY (`owner`) REFERENCES `liberdb`.`members`(`mail`) ON DELETE CASCADE ON UPDATE CASCADE; 




CREATE DATABASE db_name CHARACTER SET latin1 COLLATE latin1_swedish_ci;

CREATE DATABASE liber DEFAULT CHARACTER SET utf8 DEFAULT COLLATE utf8_unicode_ci;



Grant:
GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER ON `liberdb`.* TO 'liberdbuser'@'localhost'; 


