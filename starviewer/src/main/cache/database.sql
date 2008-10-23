-- ATENCIÓ IMPORTANT!!!!
-- CADA VEGADA QUE S'ACTUALITZI AQUEST SCRIPT, RECORDEU DE GENERAR UN SCRIPT PER ACTUALITZAR
-- LA BASE DE DADES EXISTENTS D'USUARIS D'STARVIEWER AMB ELS NOUS CANVIS I A ACTUALITZAR EL NUMERO 
-- DE REVISIO DE BASE DE DADES NECESSARIA PER LA COMPILACIÓ D'STARVIEWER ACTUA A core/starviewerapplication.h
-- A LA CONSTANT StarviewerDatabaseRevisionRequired

CREATE TABLE DatabaseRevision
(
  Revision TEXT
); 

INSERT INTO DatabaseRevision (Revision) VALUES ('$Rev');

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
  PatientAge			TEXT,
  PatientWeigth 		REAL,
  PatientHeigth 		REAL,
  Modalities			TEXT,
  Date				TEXT,
  Time				TEXT,
  AccessionNumber		TEXT,
  Description			TEXT,
  ReferringPhysicianName	TEXT,
  LastAccessDate		TEXT,
  RetrievedDate			TEXT,
  RetrievedTime			TEXT,
  State				INTEGER	
);

CREATE TABLE Series
(
  InstanceUID			TEXT PRIMARY KEY,
  StudyInstanceUID		TEXT,
  Number			TEXT,
  Modality			TEXT,
  Date				TEXT,
  Time				TEXT,
  InstitutionName		TEXT,
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
  RetrievedDate			TEXT,
  RetrievedTime			TEXT,
  State				INTEGER
);

CREATE INDEX  IndexSeries_StudyInstanceUID ON Series (StudyInstanceUID); 

CREATE TABLE Image
(
  SOPInstanceUID		TEXT PRIMARY KEY,
  StudyInstanceUID		TEXT,
  SeriesInstanceUID		TEXT,
  InstanceNumber		TEXT,
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
  RetrievedDate			TEXT,
  RetrievedTime			TEXT,
  State				INTEGER		
);

CREATE INDEX  IndexImage_StudyInstanceUIDSeriesInstanceUID ON Image (StudyInstanceUID,SeriesInstanceUID); 
