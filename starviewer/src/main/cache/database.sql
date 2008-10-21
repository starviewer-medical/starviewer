-- ATENCIÓ IMPORTANT!!!!
-- CADA VEGADA QUE S'ACTUALITZI AQUEST SCRIPT, RECORDEU D'ACTUALITZAR EL INSERT D'AQUEST SCRIPT 
-- ON INDIQUEM NUMERO DE REVISIO EN QUE S'APLIQUEN ELS CANVIS  I A GENERAR UN SCRIPT PER ACTUALITZAR
-- LA BASE DE DADES EXISTENTS AMB ELS NOUS CANVIS I FER L' UPDATE DEL NUMERO DE REVISIÓ 
-- DES DEL CODI DE L'STARVIEWER

CREATE TABLE DatabaseRevision
(
  Revision INTEGER
); 

INSERT INTO DatabaseRevision (Revision) VALUES (3078);

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
