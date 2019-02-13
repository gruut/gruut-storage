-- --------------------------------------------------------
-- 호스트:                          127.0.0.1
-- 서버 버전:                        10.3.10-MariaDB - mariadb.org binary distribution
-- 서버 OS:                        Win64
-- HeidiSQL 버전:                  9.4.0.5125
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


-- thevaulters 데이터베이스 구조 내보내기
CREATE DATABASE IF NOT EXISTS `thevaulters` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `thevaulters`;

-- 테이블 thevaulters.block 구조 내보내기
CREATE TABLE IF NOT EXISTS `block` (
  `block_id` int(20) DEFAULT NULL,
  `block_hash` varchar(100) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 테이블 데이터 thevaulters.block:~2 rows (대략적) 내보내기
/*!40000 ALTER TABLE `block` DISABLE KEYS */;
INSERT INTO `block` (`block_id`, `block_hash`) VALUES
	(2, '1111df2207d99a74fbe169e3eba035e633b65d94'),
	(1, 'cf23df2207d99a74fbe169e3eba035e633b65d94');
/*!40000 ALTER TABLE `block` ENABLE KEYS */;

-- 테이블 thevaulters.layer 구조 내보내기
CREATE TABLE IF NOT EXISTS `layer` (
  `record_id` int(20) NOT NULL AUTO_INCREMENT,
  `block_id` int(20) DEFAULT NULL,
  `sc_contents` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`record_id`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8;

-- 테이블 데이터 thevaulters.layer:~6 rows (대략적) 내보내기
/*!40000 ALTER TABLE `layer` DISABLE KEYS */;
INSERT INTO `layer` (`record_id`, `block_id`, `sc_contents`) VALUES
	(1, 1, '{"kind_of_transaction":"money_transfer","from_user_id":"mizno","from_var_type":"coin","from_var_name":"gru","to_user_id":"mang","to_var_type":"coin","to_var_name":"gru","value":20000}'),
	(2, 1, '{"kind_of_transaction":"money_transfer","from_user_id":"mizno","from_var_type":"coin","from_var_name":"gru","to_user_id":"mang","to_var_type":"coin","to_var_name":"gru","value":10000}'),
	(3, 1, '{"kind_of_transaction":"money_transfer","from_user_id":"mizno","from_var_type":"coin","from_var_name":"gru","to_user_id":"mang","to_var_type":"coin","to_var_name":"gru","value":30000}'),
	(4, 1, '{"kind_of_transaction":"money_transfer","from_user_id":"mizno","from_var_type":"coin","from_var_name":"gru","to_user_id":"mang","to_var_type":"coin","to_var_name":"gru","value":40000}'),
	(15, 1, '{"kind_of_transaction":"money_transfer","from_user_id":"temp","from_var_type":"coin","from_var_name":"gru","to_user_id":"mizno","to_var_type":"coin","to_var_name":"gru","value":2}');
/*!40000 ALTER TABLE `layer` ENABLE KEYS */;

-- 테이블 thevaulters.ledger 구조 내보내기
CREATE TABLE IF NOT EXISTS `ledger` (
  `record_id` int(20) NOT NULL AUTO_INCREMENT,
  `block_id` int(20) DEFAULT NULL,
  `user_id` varchar(20) DEFAULT NULL,
  `var_type` varchar(20) DEFAULT NULL,
  `var_name` varchar(20) DEFAULT NULL,
  `var_value` varchar(20) DEFAULT NULL,
  `merkle_path` int(20) DEFAULT NULL,
  PRIMARY KEY (`record_id`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=latin1;

-- 테이블 데이터 thevaulters.ledger:~10 rows (대략적) 내보내기
/*!40000 ALTER TABLE `ledger` DISABLE KEYS */;
INSERT INTO `ledger` (`record_id`, `block_id`, `user_id`, `var_type`, `var_name`, `var_value`, `merkle_path`) VALUES
	(1, 1, 'mizno', 'string', 'user_name', 'changhun', 36545),
	(2, 1, 'mizno', 'coin', 'btc', '100', 28222),
	(3, 1, 'mizno', 'coin', 'gru', '160000', 37876),
	(4, 1, 'mang', 'string', 'user_name', 'daehong', 14958),
	(5, 1, 'mang', 'coin', 'eth', '300', 12527),
	(6, 1, 'mang', 'coin', 'btc', '123456789', 20203),
	(7, 1, 'mang', 'coin', 'gru', '50', 12487),
	(8, 1, 'kjh', 'coin', 'gru', '100000', 24189),
	(9, 1, 'kjh', 'string', 'user_name', 'jeonghyeon', 87),
	(13, 1, 'mizno', 'coin', 'eth', '400', 52865),
	(16, 8, 'temp', 'coin', 'gru', '150', 11246);
/*!40000 ALTER TABLE `ledger` ENABLE KEYS */;

-- 테이블 thevaulters.transaction 구조 내보내기
CREATE TABLE IF NOT EXISTS `transaction` (
  `record_id` int(20) NOT NULL AUTO_INCREMENT,
  `block_id` int(20) DEFAULT NULL,
  `req_rst` varchar(20) DEFAULT NULL,
  `sc_contents` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`record_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

-- 테이블 데이터 thevaulters.transaction:~2 rows (대략적) 내보내기
/*!40000 ALTER TABLE `transaction` DISABLE KEYS */;
INSERT INTO `transaction` (`record_id`, `block_id`, `req_rst`, `sc_contents`) VALUES
	(2, 1, 'req', '{"kind_of_transaction":"money_transfer","from_user_id":"mizno","from_var_type":"coin","from_var_name":"gru","to_user_id":"mang","to_var_type":"coin","to_var_name":"gru","value":10000}'),
	(3, 2, 'req', '{"kind_of_transaction":"money_transfer","from_user_id":"mizno","from_var_type":"coin","from_var_name":"gru","to_user_id":"mang","to_var_type":"coin","to_var_name":"gru","value":500}');
/*!40000 ALTER TABLE `transaction` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
