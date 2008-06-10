CREATE TABLE Patient
( PatID		VARCHAR2(64) PRIMARY KEY,
  PatNam 	VARCHAR2(64),
  PatBirDat	VARCHAR2(8),
  PatSex	VARCHAR2(2)
);

CREATE TABLE Study
(
  PatID		VARCHAR2(64) NOT NULL,
  StuInsUID	VARCHAR2(64) PRIMARY KEY,
  PatAge	VARCHAR2(4) DEFAULT "000Y",
  StuID		VARCHAR2(16) NOT NULL,
  StuDat	VARCHAR2(8),
  StuTim	VARCHAR2(8),
  RefPhyNam	VARCHAR2(64),
  AccNum	VARCHAR2(16),
  StuDes	VARCHAR2(64),
  Modali	VARCHAR2(64),
  OpeNam	VARCHAR2(64),
  Locati	VARCHAR2(64),
  AccDat	NUMBER(8),
  AccTim	NUMBER(4),
  AbsPath	VARCHAR2(1024),
  Status	VARCHAR2(16),
  PacsID        INTEGER NOT NULL
);

CREATE INDEX IND_STUDY ON Study (StuInsUID);

CREATE TABLE Series
( SerInsUID	VARCHAR2(64) PRIMARY KEY,
  SerNum	VARCHAR2(12),
  StuInsUID	VARCHAR2(64),
  SerMod	VARCHAR2(16),
  ProNam	VARCHAR2(16),
  SerDes	VARCHAR2(64),
  SerPath 	VARCHAR2(1024),
  SerDat	VARCHAR2(8),
  SerTim	VARCHAR2(8), 
  BodParExa 	VARCHAR2(16)
);

CREATE INDEX IND_SERIES1 ON Series (StuInsUID,SerInsUID);
CREATE INDEX IND_SERIES2 ON Series (SerInsUID);

CREATE TABLE Image
(
   SOPInsUID	VARCHAR2(64) PRIMARY KEY,
   StuInsUID    VARCHAR2(64),
   SerInsUID	VARCHAR2(64),
   ImgNum	NUMBER(5),
   ImgTim	VARCHAR2(8),
   ImgDat	VARCHAR2(8),
   ImgSiz	NUMBER(10),
   ImgNam	VARCHAR2(255)	
);

CREATE INDEX IND_IMAGE1 ON Image (StuInsUID,SerInsUID);
CREATE INDEX IND_IMAGE2 ON Image (SerInsUID);


Create table PacsList
(
   PacsID       INTEGER PRIMARY KEY,
   AETitle      VARCHAR2(64) UNIQUE,
   Server       VARCHAR2(64),
   Port         VARCHAR2(6),
   Inst         VARCHAR2(100),
   Loc          VARCHAR2(100),
   Desc         VARCHAR2(255),
   Def          VARCHAR2(1),
   Del          VARCHAR2(1)
);


create table Pool
(
    Param       VARCHAR2(12) UNIQUE,
    Space       NUMBER (16)
);

insert into Pool Values ('POOLSIZE',32212254720);
insert into Pool Values ('USED',0);