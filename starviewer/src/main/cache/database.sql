CREATE TABLE Patient
( 
  ID				TEXT PRIMARY KEY,
  Name	 			TEXT,
  BirthDate			TEXT,
  Sex				TEXT
);


CREATE TABLE Study
(
  InstanceUID			TEXT PRIMARY KEY,
  PatientID			TEXT NOT NULL,
  ID				TEXT NOT NULL,
  PatientAge			INTEGER,
  PatientWeigth 		INTEGER,
  PatientHeigth 		INTEGER,
  Modalities			TEXT,
  Date				TEXT,
  Time				TEXT,
  AccessionNumber		TEXT,
  Description			TEXT,
  ReferringPhysicianName	TEXT,
  LastAccessDate		TEXT,
  State				INTEGER	
);

CREATE TABLE Series
(
  InstanceUID			TEXT PRIMARY KEY,
  StudyInstanceUID		TEXT,
  Number			INTEGER,
  Modality			TEXT,
  Date				TEXT,
  Time				TEXT,
  InstituionName		TEXT,
  PatientPosition		TEXT,
  ProtocolName			TEXT,
  Description			TEXT,
  FrameOfReferenceUID		TEXT,
  PositionReferenceIndicator	TEXT,
  NumberOfPhases		INTEGER,
  NumberOfSlicesPerPhase	INTEGER,
  BodyPartExaminated		TEXT,
  ViewPosition			TEXT,
  Manufacturer			TEXT,
  State				INTEGER
);

CREATE INDEX  IndexSeries_StudyInstanceUID ON Series (StudyInstanceUID); 

CREATE TABLE Image
(
  SOPInstanceUID		TEXT PRIMARY KEY,
  StudyInstanceUID		TEXT,
  SeriesInstanceUID		TEXT,
  InstanceNumber		INTEGER,
  ImageOrientationPatient	TEXT,
  PatientOrientation		TEXT,
  PixelSpacing			TEXT,
  SliceThickness		REAL,
  PatientPosition		TEXT,
  SamplesPerPixel		INTEGER,
  Rows				INTEGER,
  Columns			INTEGER,
  BitsAllocated 		INTEGER,
  BitsStored			INTEGER,
  PixelRepresentation		INTEGER,
  RescaleSlope			REAL,
  WindowLevelWidth		TEXT,
  WindowLevelCenter		TEXT,
  WindowLevelExplanations	TEXT,
  SOPInstanceReferenceImage	TEXT,
  SliceLocation 		TEXT,
  RescaleIntercept		REAL,
  NumberOfFrames		INTEGER,
  PhotometricInterpretation	TEXT,
  OrderNumberInSeries		INTEGER,
  State				INTEGER		
);

CREATE INDEX  IndexImage_StudyInstanceUIDSeriesInstanceUID ON Image (StudyInstanceUID,SeriesInstanceUID); 


CREATE TABLE PatientOld
( PatID		VARCHAR2(64) PRIMARY KEY,
  PatNam 	VARCHAR2(64),
  PatBirDat	VARCHAR2(8),
  PatSex	VARCHAR2(2)
);

CREATE TABLE StudyOld
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

CREATE INDEX IND_STUDYOLD ON StudyOld (StuInsUID);

CREATE TABLE SeriesOld
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

CREATE INDEX IND_SERIES1OLD ON SeriesOld (StuInsUID,SerInsUID);
CREATE INDEX IND_SERIES2OLD ON SeriesOld (SerInsUID);

CREATE TABLE ImageOld
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

CREATE INDEX IND_IMAGE1OLD ON ImageOld (StuInsUID,SerInsUID);
CREATE INDEX IND_IMAGE2OLD ON ImageOld (SerInsUID);

create table PoolOld
(
    Param       VARCHAR2(12) UNIQUE,
    Space       NUMBER (16)
);

insert into PoolOld Values ('POOLSIZE',32212254720);
insert into PoolOld Values ('USED',0);