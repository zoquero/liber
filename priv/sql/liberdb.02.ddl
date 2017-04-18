-- phpMyAdmin SQL Dump
-- version 4.2.12deb2+deb8u1build0.15.04.1
-- http://www.phpmyadmin.net
--
-- Servidor: localhost
-- Tiempo de generación: 12-03-2016 a las 16:42:39
-- Versión del servidor: 5.6.28-0ubuntu0.15.04.1
-- Versión de PHP: 5.6.4-4ubuntu6.4

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de datos: `liberdb`
--

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `ads`
--

CREATE TABLE IF NOT EXISTS `ads` (
`id` int(11) NOT NULL,
  `owner` int(11) NOT NULL,
  `isbn` bigint(20) NOT NULL COMMENT '13 digits: http://www.isbn.org/faqs_general_questions#isbn_faq3',
  `status` tinyint(4) NOT NULL,
  `summary` varchar(200) NOT NULL,
  `description` varchar(255) NOT NULL,
  `ad_publishing_date` datetime NOT NULL,
  `image` varchar(255) NOT NULL,
  `grade` tinyint(4) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COMMENT='Taula amb els anuncis de llibres a socialitzar';

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `members`
--

CREATE TABLE IF NOT EXISTS `members` (
`id` int(11) NOT NULL,
  `mail` varchar(150) NOT NULL,
  `fullname` varchar(255) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8 COMMENT='Socis actius de l''AMPA';

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `interests`
--

CREATE TABLE IF NOT EXISTS `interests` (
`id` int(11) NOT NULL,
  `ad` int(11) NOT NULL,
  `interested` int(11) NOT NULL,
  `description` varchar(255) NOT NULL,
  `date` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Interessos en llibres anunciats';

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `grades`
--

CREATE TABLE IF NOT EXISTS `grades` (
`id` tinyint(4) NOT NULL,
  `name` varchar(120) NOT NULL,
  `enabled` BOOL NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COMMENT='Cursos, com 1er d Infantil, ...';

-- --------------------------------------------------------

--
-- Índices para tablas volcadas
--

--
-- Indices de la tabla `ads`
--
ALTER TABLE `ads`
 ADD PRIMARY KEY (`id`), ADD KEY `owner` (`owner`), ADD KEY `isbn` (`isbn`), ADD KEY `status` (`status`), ADD KEY `grade` (`grade`), ADD FULLTEXT KEY `ftsearch` (`summary`,`description`) COMMENT 'full text search de textos';
-- ADD PRIMARY KEY (`id`), ADD KEY `owner` (`owner`,`isbn`,`status`,`summary`(191),`description`(191));

--
-- Indices de la tabla `members`
--
ALTER TABLE `members`
 ADD PRIMARY KEY (`id`), ADD UNIQUE KEY `mail` (`mail`);

--
-- Indices de la tabla `grades`
--
ALTER TABLE `grades`
 ADD PRIMARY KEY (`id`);

--
-- Indices de la tabla `interests`
--
ALTER TABLE `interests`
 ADD PRIMARY KEY (`id`), ADD KEY `ad` (`ad`), ADD KEY `interested` (`interested`);

--
-- AUTO_INCREMENT de las tablas volcadas
--

--
-- AUTO_INCREMENT de la tabla `ads`
--
ALTER TABLE `ads`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=20;
--
-- AUTO_INCREMENT de la tabla `grades`
--
ALTER TABLE `grades`
MODIFY `id` tinyint(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=6;
--
-- AUTO_INCREMENT de la tabla `members`
--
ALTER TABLE `members`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=20;
--
-- AUTO_INCREMENT de la tabla `interests`
--
ALTER TABLE `interests`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
--
-- Restricciones para tablas volcadas
--

--
-- Filtros para la tabla `ads`
--
ALTER TABLE `ads`
ADD CONSTRAINT `memberfk` FOREIGN KEY (`owner`) REFERENCES `members` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;
ALTER TABLE `ads`
ADD CONSTRAINT `gradefk` FOREIGN KEY (`grade`) REFERENCES `grades` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Filtros para la tabla `interests`
--
ALTER TABLE `interests`
ADD CONSTRAINT `fkAd` FOREIGN KEY (`ad`) REFERENCES `ads` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
ADD CONSTRAINT `fkMember` FOREIGN KEY (`interested`) REFERENCES `members` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
