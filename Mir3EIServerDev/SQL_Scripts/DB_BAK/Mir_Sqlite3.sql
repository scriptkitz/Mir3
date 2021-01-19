/*
 Navicat Premium Data Transfer

 Source Server         : MirSqlite
 Source Server Type    : SQLite
 Source Server Version : 3021000
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3021000
 File Encoding         : 65001

 Date: 19/01/2021 13:39:39
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for TBL_ACCOUNT
-- ----------------------------
DROP TABLE IF EXISTS "TBL_ACCOUNT";
CREATE TABLE "TBL_ACCOUNT" (
  "FLD_LOGINID" text(10) NOT NULL,
  "FLD_PASSWORD" text(10) NOT NULL,
  "FLD_USERNAME" text(20) NOT NULL,
  "FLD_CERTIFICATION" integer,
  CONSTRAINT "PK_ACCOUNT" PRIMARY KEY ("FLD_LOGINID")
);

-- ----------------------------
-- Table structure for TBL_ACCOUNTADD
-- ----------------------------
DROP TABLE IF EXISTS "TBL_ACCOUNTADD";
CREATE TABLE "TBL_ACCOUNTADD" (
  "FLD_LOGINID" text(10) NOT NULL,
  "FLD_SSNO" text(14) NOT NULL,
  "FLD_BIRTHDAY" text(10) NOT NULL,
  "FLD_PHONE" text(14) NOT NULL,
  "FLD_MOBILEPHONE" text(13) NOT NULL,
  "FLD_ADDRESS1" text(20),
  "FLD_ADDRESS2" text(20),
  "FLD_EMAIL" text(40),
  "FLD_QUIZ1" text(20),
  "FLD_ANSWER1" text(20),
  "FLD_QUIZ2" text(20),
  "FLD_ANSWER2" text(20),
  CONSTRAINT "PK_ACCOUNTADD" PRIMARY KEY ("FLD_LOGINID")
);

-- ----------------------------
-- Table structure for TBL_CHARACTER
-- ----------------------------
DROP TABLE IF EXISTS "TBL_CHARACTER";
CREATE TABLE "TBL_CHARACTER" (
  "FLD_LOGINID" text(10) NOT NULL,
  "FLD_CHARNAME" text(20) NOT NULL,
  "FLD_JOB" text(1) NOT NULL,
  "FLD_GENDER" text(1) NOT NULL,
  "FLD_LEVEL" integer NOT NULL,
  "FLD_DIRECTION" integer NOT NULL,
  "FLD_CX" integer NOT NULL,
  "FLD_CY" integer NOT NULL,
  "FLD_MAPNAME" text(16) NOT NULL,
  "FLD_GOLD" integer NOT NULL,
  "FLD_DRESS_ID" text(11) NOT NULL,
  "FLD_WEAPON_ID" text(11) NOT NULL,
  "FLD_LEFTHAND_ID" text(11) NOT NULL,
  "FLD_RIGHTHAND_ID" text(11) NOT NULL,
  "FLD_HELMET_ID" text(11) NOT NULL,
  "FLD_NECKLACE_ID" text(11) NOT NULL,
  "FLD_ARMRINGL_ID" text(11) NOT NULL,
  "FLD_ARMRINGR_ID" text(11) NOT NULL,
  "FLD_RINGL_ID" text(11) NOT NULL,
  "FLD_RINGR_ID" text(11) NOT NULL,
  CONSTRAINT "PK_CHARACTER" PRIMARY KEY ("FLD_LOGINID", "FLD_CHARNAME")
);

-- ----------------------------
-- Table structure for TBL_CHARACTER_ITEM
-- ----------------------------
DROP TABLE IF EXISTS "TBL_CHARACTER_ITEM";
CREATE TABLE "TBL_CHARACTER_ITEM" (
  "FLD_LOGINID" text(10) NOT NULL,
  "FLD_CHARNAME" text(20) NOT NULL,
  "FLD_ITEM1" text(12),
  "FLD_ITEM2" text(12),
  "FLD_ITEM3" text(12),
  "FLD_ITEM4" text(12),
  "FLD_ITEM5" text(12),
  "FLD_ITEM6" text(12),
  "FLD_ITEM7" text(12),
  "FLD_ITEM8" text(12),
  "FLD_ITEM9" text(12),
  "FLD_ITEM10" text(12),
  "FLD_ITEM11" text(12),
  "FLD_ITEM12" text(12),
  "FLD_ITEM13" text(12),
  "FLD_ITEM14" text(12),
  "FLD_ITEM15" text(12),
  "FLD_ITEM16" text(12),
  "FLD_ITEM17" text(12),
  "FLD_ITEM18" text(12),
  "FLD_ITEM19" text(12),
  "FLD_ITEM20" text(12),
  "FLD_ITEM21" text(12),
  "FLD_ITEM22" text(12),
  "FLD_ITEM23" text(12),
  "FLD_ITEM24" text(12),
  "FLD_ITEM25" text(12),
  "FLD_ITEM26" text(12),
  "FLD_ITEM27" text(12),
  "FLD_ITEM28" text(12),
  "FLD_ITEM29" text(12),
  "FLD_ITEM30" text(12),
  "FLD_ITEM31" text(12),
  "FLD_ITEM32" text(12),
  "FLD_ITEM33" text(12),
  "FLD_ITEM34" text(12),
  "FLD_ITEM35" text(12),
  "FLD_ITEM36" text(12),
  "FLD_ITEM37" text(12),
  "FLD_ITEM38" text(12),
  "FLD_ITEM39" text(12),
  "FLD_ITEM40" text(12),
  "FLD_ITEM41" text(12),
  "FLD_ITEM42" text(12),
  "FLD_ITEM43" text(12),
  "FLD_ITEM44" text(12),
  "FLD_ITEM45" text(12),
  "FLD_ITEM46" text(12),
  "FLD_ITEM47" text(12),
  "FLD_ITEM48" text(12),
  "FLD_ITEM49" text(12),
  "FLD_ITEM50" text(12),
  "FLD_ITEM51" text(12),
  "FLD_ITEM52" text(12),
  "FLD_ITEM53" text(12),
  "FLD_ITEM54" text(12),
  "FLD_ITEM55" text(12),
  "FLD_ITEM56" text(12),
  "FLD_ITEM57" text(12),
  "FLD_ITEM58" text(12),
  "FLD_ITEM59" text(12),
  "FLD_ITEM60" text(12),
  "FLD_ITEM61" text(12),
  "FLD_ITEM62" text(12),
  "FLD_ITEM63" text(12),
  "FLD_ITEM64" text(12),
  "FLD_ITEM65" text(12),
  "FLD_ITEM66" text(12),
  CONSTRAINT "PK_CHARACTER_ITEM" PRIMARY KEY ("FLD_LOGINID", "FLD_CHARNAME")
);

-- ----------------------------
-- Table structure for TBL_CHARACTER_MAGIC
-- ----------------------------
DROP TABLE IF EXISTS "TBL_CHARACTER_MAGIC";
CREATE TABLE "TBL_CHARACTER_MAGIC" (
  "FLD_LOGINID" text(10) NOT NULL,
  "FLD_CHARNAME" text(20) NOT NULL,
  "FLD_LEVEL0" integer,
  "FLD_USEKEY0" integer,
  "FLD_CURRTRAIN0" integer,
  "FLD_LEVEL1" integer,
  "FLD_USEKEY1" integer,
  "FLD_CURRTRAIN1" integer,
  "FLD_LEVEL2" integer,
  "FLD_USEKEY2" integer,
  "FLD_CURRTRAIN2" integer,
  "FLD_LEVEL3" integer,
  "FLD_USEKEY3" integer,
  "FLD_CURRTRAIN3" integer,
  "FLD_LEVEL4" integer,
  "FLD_USEKEY4" integer,
  "FLD_CURRTRAIN4" integer,
  "FLD_LEVEL5" integer,
  "FLD_USEKEY5" integer,
  "FLD_CURRTRAIN5" integer,
  "FLD_LEVEL6" integer,
  "FLD_USEKEY6" integer,
  "FLD_CURRTRAIN6" integer,
  "FLD_LEVEL7" integer,
  "FLD_USEKEY7" integer,
  "FLD_CURRTRAIN7" integer,
  "FLD_LEVEL8" integer,
  "FLD_USEKEY8" integer,
  "FLD_CURRTRAIN8" integer,
  "FLD_LEVEL9" integer,
  "FLD_USEKEY9" integer,
  "FLD_CURRTRAIN9" integer,
  "FLD_LEVEL10" integer,
  "FLD_USEKEY10" integer,
  "FLD_CURRTRAIN10" integer,
  "FLD_LEVEL11" integer,
  "FLD_USEKEY11" integer,
  "FLD_CURRTRAIN11" integer,
  "FLD_LEVEL12" integer,
  "FLD_USEKEY12" integer,
  "FLD_CURRTRAIN12" integer,
  "FLD_LEVEL13" integer,
  "FLD_USEKEY13" integer,
  "FLD_CURRTRAIN13" integer,
  "FLD_LEVEL14" integer,
  "FLD_USEKEY14" integer,
  "FLD_CURRTRAIN14" integer,
  "FLD_LEVEL15" integer,
  "FLD_USEKEY15" integer,
  "FLD_CURRTRAIN15" integer,
  "FLD_LEVEL16" integer,
  "FLD_USEKEY16" integer,
  "FLD_CURRTRAIN16" integer,
  "FLD_LEVEL17" integer,
  "FLD_USEKEY17" integer,
  "FLD_CURRTRAIN17" integer,
  "FLD_LEVEL18" integer,
  "FLD_USEKEY18" integer,
  "FLD_CURRTRAIN18" integer,
  "FLD_LEVEL19" integer,
  "FLD_USEKEY19" integer,
  "FLD_CURRTRAIN19" integer,
  "FLD_LEVEL20" integer,
  "FLD_USEKEY20" integer,
  "FLD_CURRTRAIN20" integer,
  "FLD_LEVEL21" integer,
  "FLD_USEKEY21" integer,
  "FLD_CURRTRAIN21" integer,
  "FLD_LEVEL22" integer,
  "FLD_USEKEY22" integer,
  "FLD_CURRTRAIN22" integer,
  "FLD_LEVEL23" integer,
  "FLD_USEKEY23" integer,
  "FLD_CURRTRAIN23" integer,
  "FLD_LEVEL24" integer,
  "FLD_USEKEY24" integer,
  "FLD_CURRTRAIN24" integer,
  "FLD_LEVEL25" integer,
  "FLD_USEKEY25" integer,
  "FLD_CURRTRAIN25" integer,
  "FLD_LEVEL26" integer,
  "FLD_USEKEY26" integer,
  "FLD_CURRTRAIN26" integer,
  "FLD_LEVEL27" integer,
  "FLD_USEKEY27" integer,
  "FLD_CURRTRAIN27" integer,
  "FLD_LEVEL28" integer,
  "FLD_USEKEY28" integer,
  "FLD_CURRTRAIN28" integer,
  "FLD_LEVEL29" integer,
  "FLD_USEKEY29" integer,
  "FLD_CURRTRAIN29" integer,
  "FLD_LEVEL30" integer,
  "FLD_USEKEY30" integer,
  "FLD_CURRTRAIN30" integer,
  CONSTRAINT "PK_CHARACTER_MAGIC" PRIMARY KEY ("FLD_LOGINID", "FLD_CHARNAME")
);

-- ----------------------------
-- Table structure for TBL_GUARD
-- ----------------------------
DROP TABLE IF EXISTS "TBL_GUARD";
CREATE TABLE "TBL_GUARD" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_ID" text(14) NOT NULL,
  "FLD_MAPNAME" text(14) NOT NULL,
  "FLD_POSX" integer NOT NULL,
  "FLD_POSY" integer NOT NULL,
  "FLD_DIRECTION" integer NOT NULL,
  "FLD_DESCRIPTION" text(100)
);

-- ----------------------------
-- Table structure for TBL_HORSE
-- ----------------------------
DROP TABLE IF EXISTS "TBL_HORSE";
CREATE TABLE "TBL_HORSE" (
  "FLD_CHARNAME" text(20) NOT NULL,
  "FLD_HORSEINDEX" text(11) NOT NULL,
  "FLD_HORSETYPE" integer NOT NULL,
  "FLD_LEVEL" integer,
  "FLD_PRICE" integer,
  "FLD_HP" integer,
  CONSTRAINT "PK_HORSE" PRIMARY KEY ("FLD_CHARNAME", "FLD_HORSEINDEX")
);

-- ----------------------------
-- Table structure for TBL_MAGIC
-- ----------------------------
DROP TABLE IF EXISTS "TBL_MAGIC";
CREATE TABLE "TBL_MAGIC" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_NAME" text(12) NOT NULL,
  "FLD_EFFECTTYPE" integer,
  "FLD_EFFECT" integer,
  "FLD_SPELL" integer,
  "FLD_POWER" integer,
  "FLD_MAXPOWER" integer,
  "FLD_DEFSPELL" integer,
  "FLD_DEFPOWER" integer,
  "FLD_DEFMAXPOWER" integer,
  "FLD_JOB" integer,
  "FLD_NEEDL1" integer,
  "FLD_L1TRAIN" integer,
  "FLD_NEEDL2" integer,
  "FLD_L2TRAIN" integer,
  "FLD_NEEDL3" integer,
  "FLD_L3TRAIN" integer,
  "FLD_DELAY" integer,
  "FLD_DESC" text(8),
  "FLD_DESCRIPTION" text(100),
  CONSTRAINT "PK_MAGIC" PRIMARY KEY ("FLD_INDEX")
);

-- ----------------------------
-- Table structure for TBL_MAPINFO
-- ----------------------------
DROP TABLE IF EXISTS "TBL_MAPINFO";
CREATE TABLE "TBL_MAPINFO" (
  "FLD_MAPFILENAME" text(14) NOT NULL,
  "FLD_MAPNAME" text(40) NOT NULL,
  "FLD_SERVERINDEX" integer NOT NULL,
  "FLD_ATTRIBUTE" integer,
  "FLD_RECALLMAPFNAME" text(14),
  "FLD_DESCRIPTION" text(100),
  CONSTRAINT "PK_MAPINFO" PRIMARY KEY ("FLD_MAPFILENAME")
);

-- ----------------------------
-- Table structure for TBL_MERCHANT
-- ----------------------------
DROP TABLE IF EXISTS "TBL_MERCHANT";
CREATE TABLE "TBL_MERCHANT" (
  "FLD_ID" integer NOT NULL,
  "FLD_MAPNAME" text(14) NOT NULL,
  "FLD_POSX" integer NOT NULL,
  "FLD_POSY" integer NOT NULL,
  "FLD_NPCNAME" text(40) NOT NULL,
  "FLD_FACE" integer NOT NULL,
  "FLD_BODY" integer NOT NULL,
  "FLD_GENDER" integer NOT NULL,
  "FLD_DESCRIPTION" text(100),
  CONSTRAINT "PK_MERCHANT" PRIMARY KEY ("FLD_ID")
);

-- ----------------------------
-- Table structure for TBL_MONGEN
-- ----------------------------
DROP TABLE IF EXISTS "TBL_MONGEN";
CREATE TABLE "TBL_MONGEN" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_MAPNAME" text(14) NOT NULL,
  "FLD_X" integer NOT NULL,
  "FLD_Y" integer,
  "FLD_MONNAME" text(14),
  "FLD_AREA" integer,
  "FLD_COUNT" integer,
  "FLD_GENTIME" integer,
  "FLD_SMALLGENRATE" integer,
  "FLD_DESCRIPTION" text(100),
  CONSTRAINT "PK_MONGEN" PRIMARY KEY ("FLD_INDEX")
);

-- ----------------------------
-- Table structure for TBL_MONSTER
-- ----------------------------
DROP TABLE IF EXISTS "TBL_MONSTER";
CREATE TABLE "TBL_MONSTER" (
  "FLD_NAME" text(14) NOT NULL,
  "FLD_RACE" integer,
  "FLD_RACEIMG" integer,
  "FLD_APPR" integer,
  "FLD_LEVEL" integer,
  "FLD_UNDEAD" integer,
  "FLD_EXP" integer,
  "FLD_HP" integer,
  "FLD_MP" integer,
  "FLD_AC" integer,
  "FLD_MAXAC" integer,
  "FLD_MAC" integer,
  "FLD_MAXMAC" integer,
  "FLD_DC" integer,
  "FLD_MAXDC" integer,
  "FLD_SPEED" integer,
  "FLD_HIT" integer,
  "FLD_WALKSPEED" integer,
  "FLD_ATTACKSPEED" integer
);

-- ----------------------------
-- Table structure for TBL_MOVEMAPEVENT
-- ----------------------------
DROP TABLE IF EXISTS "TBL_MOVEMAPEVENT";
CREATE TABLE "TBL_MOVEMAPEVENT" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_SMAPFILENAME" text(14) NOT NULL,
  "FLD_SX" integer NOT NULL,
  "FLD_SY" integer NOT NULL,
  "FLD_DMAPFILENAME" text(14) NOT NULL,
  "FLD_DX" integer NOT NULL,
  "FLD_DY" integer NOT NULL,
  "FLD_DESCRIPTION" text(100),
  CONSTRAINT "PK_MOVEMAPEVENT" PRIMARY KEY ("FLD_INDEX")
);

-- ----------------------------
-- Table structure for TBL_NPC
-- ----------------------------
DROP TABLE IF EXISTS "TBL_NPC";
CREATE TABLE "TBL_NPC" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_NAME" text(14) NOT NULL,
  "FLD_RACE" integer NOT NULL,
  "FLD_MAPNAME" text(14) NOT NULL,
  "FLD_POSX" integer NOT NULL,
  "FLD_POSY" integer NOT NULL,
  "FLD_FACE" integer NOT NULL,
  "FLD_BODY" integer NOT NULL,
  "FLD_DESCRIPTION" text(100)
);

-- ----------------------------
-- Table structure for TBL_SERVERINFO
-- ----------------------------
DROP TABLE IF EXISTS "TBL_SERVERINFO";
CREATE TABLE "TBL_SERVERINFO" (
  "FLD_SERVERIDX" integer NOT NULL,
  "FLD_SERVERNAME" text(40) NOT NULL,
  "FLD_SERVERIP" text(20) NOT NULL,
  CONSTRAINT "PK_SERVERINFO" PRIMARY KEY ("FLD_SERVERIDX")
);

-- ----------------------------
-- Table structure for TBL_STARTPOINT
-- ----------------------------
DROP TABLE IF EXISTS "TBL_STARTPOINT";
CREATE TABLE "TBL_STARTPOINT" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_MAPNAME" text(14) NOT NULL,
  "FLD_POSX" integer NOT NULL,
  "FLD_POSY" integer NOT NULL,
  "FLD_DESCRIPTION" text(100)
);

-- ----------------------------
-- Table structure for TBL_STDITEM
-- ----------------------------
DROP TABLE IF EXISTS "TBL_STDITEM";
CREATE TABLE "TBL_STDITEM" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_NAME" text(14) NOT NULL,
  "FLD_STDMODE" integer,
  "FLD_SHAPE" integer,
  "FLD_WEIGHT" integer,
  "FLD_ANICOUNT" integer,
  "FLD_SOURCE" integer,
  "FLD_RESERVED" integer,
  "FLD_LOOKS" integer,
  "FLD_DURAMAX" integer,
  "FLD_AC" integer,
  "FLD_AC2" integer,
  "FLD_MAC" integer,
  "FLD_MAC2" integer,
  "FLD_DC" integer,
  "FLD_DC2" integer,
  "FLD_MC" integer,
  "FLD_MC2" integer,
  "FLD_SC" integer,
  "FLD_SC2" integer,
  "FLD_NEED" integer,
  "FLD_NEEDLEVEL" integer,
  "FLD_PRICE" integer,
  "FLD_STOCK" integer,
  "FLD_DESCRIPTION" text(100),
  CONSTRAINT "PK_MONSTER" PRIMARY KEY ("FLD_INDEX")
);

-- ----------------------------
-- Table structure for TBL_STDITEMARMOR
-- ----------------------------
DROP TABLE IF EXISTS "TBL_STDITEMARMOR";
CREATE TABLE "TBL_STDITEMARMOR" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_NAME" text(20) NOT NULL,
  "FLD_STDMODE" integer,
  "FLD_SHAPE" integer,
  "FLD_WEIGHT" integer,
  "FLD_ANICOUNT" integer,
  "FLD_SOURCE" integer,
  "FLD_LOOKS" integer,
  "FLD_DURAMAX" integer,
  "FLD_AC" integer,
  "FLD_AC2" integer,
  "FLD_MAC" integer,
  "FLD_MAC2" integer,
  "FLD_DC" integer,
  "FLD_DC2" integer,
  "FLD_MC" integer,
  "FLD_MC2" integer,
  "FLD_SC" integer,
  "FLD_SC2" integer,
  "FLD_NEED" integer,
  "FLD_NEEDLEVEL" integer,
  "FLD_PRICE" integer,
  "FLD_STOCK" integer,
  CONSTRAINT "PK_STDITEMARMOR" PRIMARY KEY ("FLD_INDEX")
);

-- ----------------------------
-- Table structure for TBL_STDITEMWEAPON
-- ----------------------------
DROP TABLE IF EXISTS "TBL_STDITEMWEAPON";
CREATE TABLE "TBL_STDITEMWEAPON" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_NAME" text(20) NOT NULL,
  "FLD_STDMODE" integer,
  "FLD_SHAPE" integer,
  "FLD_WEIGHT" integer,
  "FLD_ANICOUNT" integer,
  "FLD_SOURCE" integer,
  "FLD_LOOKS" integer,
  "FLD_DURAMAX" integer,
  "FLD_AC" integer,
  "FLD_AC2" integer,
  "FLD_MAC" integer,
  "FLD_MAC2" integer,
  "FLD_DC" integer,
  "FLD_DC2" integer,
  "FLD_MC" integer,
  "FLD_MC2" integer,
  "FLD_SC" integer,
  "FLD_SC2" integer,
  "FLD_NEED" integer,
  "FLD_NEEDLEVEL" integer,
  "FLD_PRICE" integer,
  "FLD_STOCK" integer,
  CONSTRAINT "PK_STDITEMWEAPON" PRIMARY KEY ("FLD_INDEX")
);

-- ----------------------------
-- Table structure for TBL_STDITEM_ACCESSORY
-- ----------------------------
DROP TABLE IF EXISTS "TBL_STDITEM_ACCESSORY";
CREATE TABLE "TBL_STDITEM_ACCESSORY" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_NAME" text(20) NOT NULL,
  "FLD_STDMODE" integer,
  "FLD_SHAPE" integer,
  "FLD_WEIGHT" integer,
  "FLD_ANICOUNT" integer,
  "FLD_SOURCE" integer,
  "FLD_LOOKS" integer,
  "FLD_DURAMAX" integer,
  "FLD_AC" integer,
  "FLD_AC2" integer,
  "FLD_MAC" integer,
  "FLD_MAC2" integer,
  "FLD_DC" integer,
  "FLD_DC2" integer,
  "FLD_MC" integer,
  "FLD_MC2" integer,
  "FLD_SC" integer,
  "FLD_SC2" integer,
  "FLD_NEED" integer,
  "FLD_NEEDLEVEL" integer,
  "FLD_PRICE" integer,
  "FLD_STOCK" integer,
  CONSTRAINT "PK_STDITEM_ACCESSORY" PRIMARY KEY ("FLD_INDEX")
);

-- ----------------------------
-- Table structure for TBL_STDITEM_ETC
-- ----------------------------
DROP TABLE IF EXISTS "TBL_STDITEM_ETC";
CREATE TABLE "TBL_STDITEM_ETC" (
  "FLD_INDEX" integer NOT NULL,
  "FLD_NAME" text(20) NOT NULL,
  "FLD_STDMODE" integer,
  "FLD_SHAPE" integer,
  "FLD_LOOKS" integer,
  "FLD_WEIGHT" integer,
  "FLD_PRICE" integer,
  "FLD_VAL1" integer,
  "FLD_VAL2" integer,
  CONSTRAINT "PK_STDITEM_ETC" PRIMARY KEY ("FLD_INDEX")
);

-- ----------------------------
-- Table structure for TBL_USERITEM
-- ----------------------------
DROP TABLE IF EXISTS "TBL_USERITEM";
CREATE TABLE "TBL_USERITEM" (
  "FLD_CHARNAME" text(20) NOT NULL,
  "FLD_STDTYPE" text(1) NOT NULL,
  "FLD_MAKEDATE" text(6) NOT NULL,
  "FLD_MAKEINDEX" text(5) NOT NULL,
  "FLD_STDINDEX" integer NOT NULL,
  "FLD_DURA" integer NOT NULL,
  "FLD_DURAMAX" integer NOT NULL,
  "FLD_VALUE1" integer NOT NULL,
  "FLD_VALUE2" integer NOT NULL,
  "FLD_VALUE3" integer NOT NULL,
  "FLD_VALUE4" integer NOT NULL,
  "FLD_VALUE5" integer NOT NULL,
  "FLD_VALUE6" integer NOT NULL,
  "FLD_VALUE7" integer NOT NULL,
  "FLD_VALUE8" integer NOT NULL,
  "FLD_VALUE9" integer NOT NULL,
  "FLD_VALUE10" integer NOT NULL,
  "FLD_VALUE11" integer NOT NULL,
  "FLD_VALUE12" integer NOT NULL,
  "FLD_VALUE13" integer NOT NULL,
  "FLD_VALUE14" integer NOT NULL,
  CONSTRAINT "PK_USERITEM" PRIMARY KEY ("FLD_MAKEINDEX")
);

-- ----------------------------
-- Table structure for sqlite_stat1
-- ----------------------------
DROP TABLE IF EXISTS "sqlite_stat1";
CREATE TABLE "sqlite_stat1" (
  "tbl",
  "idx",
  "stat"
);

-- ----------------------------
-- Indexes structure for table TBL_ACCOUNTADD
-- ----------------------------
CREATE INDEX "IDX_ACCOUNTADD_SSNO"
ON "TBL_ACCOUNTADD" (
  "FLD_SSNO" ASC
);

PRAGMA foreign_keys = true;
