revoke usage on `liberdb`.* from 'liberdbuser'@'localhost';
DROP USER 'liberdbuser'@'localhost';
DROP DATABASE IF EXISTS liberdb;
FLUSH PRIVILEGES;
