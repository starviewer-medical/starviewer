/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006-2010 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include <gdcmAnonymizer.h>
#include <gdcmGlobal.h>
#include <gdcmStringFilter.h>
#include <gdcmSequenceOfItems.h>
#include <gdcmExplicitDataElement.h>
#include <gdcmSwapper.h>
#include <gdcmDataSetHelper.h>
#include <gdcmUIDGenerator.h>
#include <gdcmAttribute.h>
#include <gdcmDummyValueGenerator.h>
#include <gdcmDicts.h>
#include <gdcmType.h>
#include <gdcmDefs.h>
#include <gdcmEvent.h>
#include <gdcmAnonymizeEvent.h>

#include "gdcmanonymizerstarviewer.h"

namespace gdcm
{

// PS 3.15 - 2008
// Table E.1-1
// BALCPA
static Tag BasicApplicationLevelConfidentialityProfileAttributes[] = {
//    Attribute Name                                Tag
/*    Instance Creator UID                      */ Tag(0x0008, 0x0014),
/*    SOP Instance UID                          */ Tag(0x0008, 0x0018),
/*    Accession Number                          */ Tag(0x0008, 0x0050),
/*    Institution Name                          */ Tag(0x0008, 0x0080),
/*    Institution Address                       */ Tag(0x0008, 0x0081),
/*    Referring Physician's Name                */ Tag(0x0008, 0x0090),
/*    Referring Physician's Address             */ Tag(0x0008, 0x0092),
/*    Referring Physician's Telephone Numbers   */ Tag(0x0008, 0x0094),
/*    Station Name                              */ Tag(0x0008, 0x1010),
/*    Study Description                         */ Tag(0x0008, 0x1030),
/*    Series Description                        */ Tag(0x0008, 0x103E),
/*    Institutional Department Name             */ Tag(0x0008, 0x1040),
/*    Physician(s) of Record                    */ Tag(0x0008, 0x1048),
/*    Performing Physicians' Name               */ Tag(0x0008, 0x1050),
/*    Name of Physician(s) Reading Study        */ Tag(0x0008, 0x1060),
/*    Operators' Name                           */ Tag(0x0008, 0x1070),
/*    Admitting Diagnoses Description           */ Tag(0x0008, 0x1080),
/*    Referenced SOP Instance UID               */ Tag(0x0008, 0x1155),
/*    Derivation Description                    */ Tag(0x0008, 0x2111),
/*    Patient's Name                            */ Tag(0x0010, 0x0010),
/*    Patient ID                                */ Tag(0x0010, 0x0020),
/*    Patient's Birth Date                      */ Tag(0x0010, 0x0030),
/*    Patient's Birth Time                      */ Tag(0x0010, 0x0032),
/*    Patient's Sex                             */ Tag(0x0010, 0x0040),
/*    Other Patient Ids                         */ Tag(0x0010, 0x1000),
/*    Other Patient Names                       */ Tag(0x0010, 0x1001),
/*    Patient's Age                             */ Tag(0x0010, 0x1010),
/*    Patient's Size                            */ Tag(0x0010, 0x1020),
/*    Patient's Weight                          */ Tag(0x0010, 0x1030),
/*    Medical Record Locator                    */ Tag(0x0010, 0x1090),
/*    Ethnic Group                              */ Tag(0x0010, 0x2160),
/*    Occupation                                */ Tag(0x0010, 0x2180),
/*    Additional Patient's History              */ Tag(0x0010, 0x21B0),
/*    Patient Comments                          */ Tag(0x0010, 0x4000),
/*    Device Serial Number                      */ Tag(0x0018, 0x1000),
/*    Protocol Name                             */ Tag(0x0018, 0x1030),
/*    Study Instance UID                        */ Tag(0x0020, 0x000D),
/*    Series Instance UID                       */ Tag(0x0020, 0x000E),
/*    Study ID                                  */ Tag(0x0020, 0x0010),
/*    Frame of Reference UID                    */ Tag(0x0020, 0x0052),
/*    Synchronization Frame of Reference UID    */ Tag(0x0020, 0x0200),
/*    Image Comments                            */ Tag(0x0020, 0x4000),
/*    Request Attributes Sequence               */ Tag(0x0040, 0x0275),
/*    UID                                       */ Tag(0x0040, 0xA124),
/*    Content Sequence                          */ Tag(0x0040, 0xA730),
/*    Storage Media File-set UID                */ Tag(0x0088, 0x0140),
/*    Referenced Frame of Reference UID         */ Tag(0x3006, 0x0024),
/*    Related Frame of Reference UID            */ Tag(0x3006, 0x00C2)
};

gdcmAnonymizerStarviewer::~gdcmAnonymizerStarviewer()
{
}

bool gdcmAnonymizerStarviewer::Empty(Tag const &t)
{
    // There is a secret code path to make it work for VR::SQ since operation is just 'make empty'
    return Replace(t, "", 0);
}

bool gdcmAnonymizerStarviewer::Remove(Tag const &t)
{
    if (t.GetGroup() < 0x0008) return false;
    DataSet &ds = F->GetDataSet();
    return ds.Remove(t) == 1;
}

bool gdcmAnonymizerStarviewer::Replace(Tag const &t, const char *value)
{
    size_t len = 0;
    if (value)
    {
        len = strlen(value);
    }
    return Replace(t, value, len);
}

bool gdcmAnonymizerStarviewer::Replace(Tag const &t, const char *value, VL const & vl)
{
    if (t.GetGroup() < 0x0008) return false;
    static const Global &g = GlobalInstance;
    static const Dicts &dicts = g.GetDicts();
    DataSet &ds = F->GetDataSet();
    // Let's do the private tag:
    bool ret = false;
    if (t.IsPrivate())
    {
        // Only one operation is allowed: making a private tag empty ...
        if (vl == 0)
        {
            if (ds.FindDataElement(t))
            {
                DataElement de (ds.GetDataElement(t));
                if (de.GetVR() != VR::INVALID)
                {
                    if (de.GetVR() == VR::SQ)
                    {
                        gdcmDebugMacro("Cannot replace a VR:SQ");
                        return false;
                    }
                }
                de.SetByteValue("", vl);
                ds.Insert(de);
                ret = true;
            }
            else
            {
                // TODO
                assert(0 && "TODO");
                ret = false;
            }
        }
    }
    else
    {
        // Ok this is a public element
        assert(t.IsPublic());
        const DictEntry &dictentry = dicts.GetDictEntry(t);
        if ( dictentry.GetVR() == VR::INVALID
          || dictentry.GetVR() == VR::UN
          || dictentry.GetVR() == VR::SQ)
        {
            // Make the VR::SQ empty
            if (dictentry.GetVR() == VR::SQ && vl == 0 && value && *value == 0)
            {
                DataElement de(t);
                de.SetVR(VR::SQ);
                //de.SetByteValue("", 0);
                ds.Replace(de);
            }
            else
            {
                // Let's give up !
                gdcmWarningMacro("Cannot process tag: " << t << " with vr: " << dictentry.GetVR());
            }
            //ret = false;
        }
        else if (dictentry.GetVR() & VR::VRBINARY)
        {
            if (vl == 0)
            {
                DataElement de(t);
                if (ds.FindDataElement(t))
                {
                    de.SetVR(ds.GetDataElement(t).GetVR());
                }
                else
                {
                    de.SetVR(dictentry.GetVR());
                }
                de.SetByteValue("", 0);
                ds.Replace(de);
                ret = true;
            }
            else
            {
                gdcmWarningMacro("You need to explicitely specify the length for this type of vr: " << dictentry.GetVR());
                ret = false;
            }
#if 0
            StringFilter sf;
            sf.SetFile(*F);
            std::string s = sf.FromString(t, value, vl);
            DataElement de(t);
            if (ds.FindDataElement(t))
            {
                de.SetVR(ds.GetDataElement(t).GetVR());
            }
            else
            {
                de.SetVR(dictentry.GetVR());
            }
            de.SetByteValue(s.c_str(), s.size());
            ds.Replace(de);
            ret = true;
#endif
        }
        else
        {
            // vr from dict seems to be ascii, so it seems resonable to write a ByteValue here:
            assert(dictentry.GetVR() & VR::VRASCII);
            if (value)
            {
                std::string padded(value, vl);
                // All ASCII VR needs to be padded with a space
                if (vl.IsOdd())
                {
                    if (dictentry.GetVR() == VR::UI)
                    {
                        // \0 is automatically added when using a ByteValue
                    }
                    else
                    {
                        padded += " ";
                    }
                }
                // Hum, we could have cases where a public element would not be known, in which case
                // it is a good idea to first check for the VR as found in the file:
                DataElement de(t);
                if (ds.FindDataElement(t))
                {
                    de.SetVR(ds.GetDataElement(t).GetVR());
                }
                else
                {
                    de.SetVR(dictentry.GetVR());
                }
                de.SetByteValue(padded.c_str(), padded.size());
                ds.Replace(de);
                ret = true;
            }
        }
    }
    return ret;
}

static bool Anonymizer_RemoveRetired(File const &file, DataSet &ds)
{
    static const Global &g = GlobalInstance;
    static const Dicts &dicts = g.GetDicts();
    static const Dict &pubdict = dicts.GetPublicDict();
    DataSet::Iterator it = ds.Begin();
    for (; it != ds.End();)
    {
        const DataElement &de1 = *it;
        // std::set::erase invalidate iterator, so we need to make a copy first:
        DataSet::Iterator dup = it;
        ++it;
        if (de1.GetTag().IsPublic())
        {
            const DictEntry &entry = pubdict.GetDictEntry(de1.GetTag());
            if (entry.GetRetired())
            {
                ds.GetDES().erase(dup);
            }
        }
        else
        {
            const DataElement &de = *dup;
            VR vr = DataSetHelper::ComputeVR(file, ds, de.GetTag());
            if (vr.Compatible(VR::SQ))
            {
                SmartPointer<SequenceOfItems> sq = de.GetValueAsSQ();
                if (sq)
                {
                    unsigned int n = sq->GetNumberOfItems();
                    for (unsigned int i = 1; i <= n; i++) // item starts at 1, not 0
                    {
                        Item &item = sq->GetItem(i);
                        DataSet &nested = item.GetNestedDataSet();
                        Anonymizer_RemoveRetired(file, nested);
                    }
                    DataElement de_dup = *dup;
                    de_dup.SetValue(*sq);
                    de_dup.SetVLToUndefined(); // FIXME
                    ds.Replace(de_dup);
                }
            }
        }
    }
    return true;
}

bool gdcmAnonymizerStarviewer::RemoveRetired()
{
    DataSet &ds = F->GetDataSet();
    return Anonymizer_RemoveRetired(*F, ds);
}

static bool Anonymizer_RemoveGroupLength(File const &file, DataSet &ds)
{
    DataSet::Iterator it = ds.Begin();
    for (; it != ds.End();)
    {
        const DataElement &de1 = *it;
        // std::set::erase invalidate iterator, so we need to make a copy first:
        DataSet::Iterator dup = it;
        ++it;
        if (de1.GetTag().IsGroupLength())
        {
            ds.GetDES().erase(dup);
        }
        else
        {
            const DataElement &de = *dup;
            VR vr = DataSetHelper::ComputeVR(file, ds, de.GetTag());
            if (vr.Compatible(VR::SQ))
            {
                SmartPointer<SequenceOfItems> sq = de.GetValueAsSQ();
                if (sq)
                {
                    unsigned int n = sq->GetNumberOfItems();
                    for (unsigned int i = 1; i <= n; i++) // item starts at 1, not 0
                    {
                        Item &item = sq->GetItem(i);
                        DataSet &nested = item.GetNestedDataSet();
                        Anonymizer_RemoveGroupLength(file, nested);
                    }
                    DataElement de_dup = *dup;
                    de_dup.SetValue(*sq);
                    de_dup.SetVLToUndefined(); // FIXME
                    ds.Replace(de_dup);
                }
            }
        }
    }
    return true;
}

bool gdcmAnonymizerStarviewer::RemoveGroupLength()
{
    DataSet &ds = F->GetDataSet();
    return Anonymizer_RemoveGroupLength(*F, ds);
}

static bool Anonymizer_RemovePrivateTags(File const &file, DataSet &ds)
{
    DataSet::Iterator it = ds.Begin();
    for (; it != ds.End();)
    {
        const DataElement &de1 = *it;
        // std::set::erase invalidate iterator, so we need to make a copy first:
        DataSet::Iterator dup = it;
        ++it;
        if (de1.GetTag().IsPrivate())
        {
            ds.GetDES().erase(dup);
        }
        else
        {
            const DataElement &de = *dup;
            VR vr = DataSetHelper::ComputeVR(file, ds, de.GetTag());
            if (vr.Compatible(VR::SQ))
            {
                SmartPointer<SequenceOfItems> sq = de.GetValueAsSQ();
                if (sq)
                {
                    unsigned int n = sq->GetNumberOfItems();
                    for (unsigned int i = 1; i <= n; i++) // item starts at 1, not 0
                    {
                        Item &item = sq->GetItem(i);
                        DataSet &nested = item.GetNestedDataSet();
                        Anonymizer_RemovePrivateTags(file, nested);
                    }
                    DataElement de_dup = *dup;
                    de_dup.SetValue(*sq);
                    de_dup.SetVLToUndefined(); // FIXME
                    ds.Replace(de_dup);
                }
            }
        }
    }
    return true;
}

bool gdcmAnonymizerStarviewer::RemovePrivateTags()
{
    DataSet &ds = F->GetDataSet();
    return Anonymizer_RemovePrivateTags(*F, ds);
}

/*
 * Implementation note:
 * In order to implement the dummy 'memory' we use a static std::map
 * this works great but we cannot be thread safe.
 * In order to be thread safe, we would need to externalize this map generation
 * maybe using a gdcm::Scanner do the operation once (Scanner is doing) the merging
 * automatically...
 * this is left as an exercise for the reader :)
 */
bool gdcmAnonymizerStarviewer::BasicApplicationLevelConfidentialityProfile(bool deidentify)
{
    this->InvokeEvent(StartEvent());
    bool ret;
    if (deidentify)
        ret = BasicApplicationLevelConfidentialityProfile1();
    this->InvokeEvent(EndEvent());
    return ret;
}

std::vector<Tag> gdcmAnonymizerStarviewer::GetBasicApplicationLevelConfidentialityProfileAttributes()
{
    static const unsigned int deidSize = sizeof(Tag);
    static const unsigned int numDeIds = sizeof(BasicApplicationLevelConfidentialityProfileAttributes) / deidSize;
    static const Tag *start = BasicApplicationLevelConfidentialityProfileAttributes;
    static const Tag *end = start + numDeIds;
    return std::vector<Tag>(start, end);
}

bool gdcmAnonymizerStarviewer::CheckIfSequenceContainsAttributeToAnonymize(File const &file, SequenceOfItems *sqi) const
{
    static const unsigned int deidSize = sizeof(Tag);
    static const unsigned int numDeIds = sizeof(BasicApplicationLevelConfidentialityProfileAttributes) / deidSize;
    static const Tag *start = BasicApplicationLevelConfidentialityProfileAttributes;
    static const Tag *end = start + numDeIds;

    bool found = false;
    for (const Tag *ptr = start; ptr != end && !found; ++ptr)
    {
        const Tag& tag = *ptr;
        found = sqi->FindDataElement(tag);
    }
    // ok we can exit.
    if (found) return true;

    // now look into sub-sequence:
    unsigned int n = sqi->GetNumberOfItems();
    for (unsigned int i = 1; i <= n; i++) // item starts at 1, not 0
    {
        Item &item = sqi->GetItem(i);
        DataSet &nested = item.GetNestedDataSet();
        DataSet::Iterator it = nested.Begin();
        for (; it != nested.End() && !found; ++it)
        {
            const DataElement &de = *it;
            VR vr = DataSetHelper::ComputeVR(file, nested, de.GetTag());
            SmartPointer<SequenceOfItems> sqi2 = 0;
            if (vr == VR::SQ)
            {
                sqi2 = de.GetValueAsSQ();
            }
            if (sqi2)
            {
                found = CheckIfSequenceContainsAttributeToAnonymize(file, sqi2);
            }
        }
    }

    return found;
}

// Implementation note:
// This function trigger:
// 1 StartEvent
// 1 EndEvent
// 6 IterationEvent
// N AnonymizeEvent (depend on number of tag found)
bool gdcmAnonymizerStarviewer::BasicApplicationLevelConfidentialityProfile1()
{
    DataSet &ds = F->GetDataSet();
    if ( ds.FindDataElement(Tag(0x0400, 0x0500))
      || ds.FindDataElement(Tag(0x0012, 0x0062))
      || ds.FindDataElement(Tag(0x0012, 0x0063)))
    {
        gdcmDebugMacro("EncryptedContentTransferSyntax Attribute is present !");
        return false;
    }

  // PS 3.15
  // E.1 BASIC APPLICATION LEVEL CONFIDENTIALITY PROFILE
  // An Application may claim conformance to the Basic Application Level Confidentiality Profile as a deidentifier
  // if it protects all Attributes that might be used by unauthorized entities to identify the patient.
  // Protection in this context is defined as the following process:

  // 2. Each Attribute to be protected shall then either be removed from the dataset, or have its value
  // replaced by a different "replacement value" which does not allow identification of the patient.

  //Aquest codi ha estat comentat per GDCM
  //for (const Tag *ptr = start ; ptr != end ; ++ptr)
  //  {
  //  const Tag& tag = *ptr;
  //  // FIXME Type 1 !
  //  if (ds.FindDataElement(tag)) BALCPProtect(F->GetDataSet(), tag);
  //  }
  // Check that root level sequence do not contains any of those attributes
    try
    {
        RecurseDataSet(F->GetDataSet());
    }
    catch(std::exception &ex)
    {
        gdcmDebugMacro("Problem during RecurseDataSet");
        return false;
    }
    catch(...)
    {
        gdcmDebugMacro("Unknown Problem during RecurseDataSet");
        return false;
    }

    this->InvokeEvent(IterationEvent());

    // Group Length are removed since PS 3.3-2008
    RemoveGroupLength();

    // 3. At the discretion of the de-identifier, Attributes may be added to the dataset to be protected.
    // ...

    // 6. The attribute Patient Identity Removed (0012,0062) shall be replaced or added to the dataset with a
    // value of YES, and a value inserted in De-identification Method (0012,0063) or De-identification
    // Method Code Sequence (0012,0064).
    Replace(Tag(0x0012, 0x0062), "YES");
    Replace(Tag(0x0012, 0x0063), "BASIC APPLICATION LEVEL CONFIDENTIALITY PROFILE");

    this->InvokeEvent(IterationEvent());

    return true;
}

bool IsVRUI(Tag const &tag)
{
    static const Global &g = Global::GetInstance();
    static const Dicts &dicts = g.GetDicts();
    const DictEntry &dictentry = dicts.GetDictEntry(tag);
    if (dictentry.GetVR() == VR::UI) return true;
    //if (tag == Tag(0x0020,0x000d)   // Study Instance UID : UI
    // || tag == Tag(0x0020,0x0052)   //
    // || tag == Tag(0x0020,0x000e)) // Series Instance UID : UI
    //  {
    //  return true;
    //  }
    return false;
}

static const Tag SpecialTypeTags[] = {
/*   Patient's Name          */ Tag(0x0010, 0x0010),
/*   Patient ID              */ Tag(0x0010, 0x0020),
/*   Study ID                */ Tag(0x0020, 0x0010),
/*   Series Number           */ Tag(0x0020, 0x0011)
};

bool gdcmAnonymizerStarviewer::CanEmptyTag(Tag const &tag, const IOD &iod) const
{
    static const Global &g = Global::GetInstance();
    //static const Dicts &dicts = g.GetDicts();
    static const Defs &defs = g.GetDefs();
    const DataSet &ds = F->GetDataSet();
    (void)ds;
    //Type told = defs.GetTypeFromTag(*F, tag);
    Type t = iod.GetTypeFromTag(defs, tag);
    //assert(t == told);

    gdcmDebugMacro("Type for tag=" << tag << " is " << t);

    //assert(t != Type::UNKNOWN);

    if (t == Type::T1 || t == Type::T1C)
    {
        return false;
    }
    // What if we are dealing with a Standard Extended SOP class
    // eg. gdcmData/05115014-mr-siemens-avanto-syngo-with-palette-icone.dcm
    // where Attribute is not present in standard DICOM IOD - (0x0088,0x0140) UI Storage Media FileSet UID
    if (t == Type::UNKNOWN)
    {
        return true;
    }

    // http://groups.google.com/group/comp.protocols.dicom/browse_thread/thread/b1b23101bb655b81
    /*
    ...
    3. It is the responsibility of the de-identifier to ensure the
    consistency of dummy values for Attributes
    such as Study Instance UID (0020,000D) or Frame of Reference UID
    (0020,0052) if multiple related
    SOP Instances are protected.
    ...

    I think it would also make sense to quote the following attributes:
    * Patient ID,
    * Study ID,
    * Series Number.
    It is required they have consistent values when one is about to
    generate a DICOMDIR

    => Sup 142
    */
    static const unsigned int deidSize = sizeof(Tag);
    static const unsigned int numDeIds = sizeof(SpecialTypeTags) / deidSize;

    bool b = std::binary_search(SpecialTypeTags, SpecialTypeTags + numDeIds, tag);

    // This is a Type 3 attribute but with VR=UI
    // <entry group="0008" element="0014" vr="UI" vm="1" name="Instance Creator UID"/>
    //assert(dicts.GetDictEntry(tag).GetVR() != VR::UI);
    return !b;
}

bool gdcmAnonymizerStarviewer::BALCPProtect(DataSet &ds, Tag const & tag, IOD const & iod)
{
    // \precondition
    assert(ds.FindDataElement(tag));

    AnonymizeEvent ae;
    ae.SetTag(tag);
    this->InvokeEvent(ae);

    typedef std::pair<Tag, std::string> TagValueKey;
    typedef std::map<TagValueKey, std::string> DummyMapNonUIDTags;
    typedef std::map<std::string, std::string> DummyMapUIDTags;
    static DummyMapNonUIDTags dummyMapNonUIDTags;
    static DummyMapUIDTags dummyMapUIDTags;

    bool canempty = CanEmptyTag(tag, iod);
    if (!canempty)
    {
        DataElement copy;
        copy = ds.GetDataElement(tag);

        if (IsVRUI(tag))
        {
            std::string UIDToAnonymize = "";
            gdcm::UIDGenerator uid;

            if (!copy.IsEmpty())
            {
                if (ByteValue *bv = copy.GetByteValue())
                {
                    UIDToAnonymize = std::string(bv->GetPointer(), bv->GetLength());
                }
            }

            std::string anonymizedUID = "";
            if (!UIDToAnonymize.empty())
            {
                if (dummyMapUIDTags.count(UIDToAnonymize) == 0)
                {
                    anonymizedUID = uid.Generate();
                    dummyMapUIDTags[UIDToAnonymize] = anonymizedUID;
                    //INFO_LOG(QString("Busquem valor per tag %1,%2").arg(tag.GetGroup()).arg(tag.GetElement()) + QString("Genero nou valor per %1 el nou valor es %2").arg(UIDValue.c_str()).arg(anonymizedUID.c_str()));
                }
                else
                {
                    anonymizedUID = dummyMapUIDTags[UIDToAnonymize];
                    //INFO_LOG(QString("Busquem valor per tag %1,%2").arg(tag.GetGroup()).arg(tag.GetElement()) + QString("Trobo valor per %1 el seu valor es %2").arg(UIDValue.c_str()).arg(anonymizedUID.c_str()));
                }
            }
            else
            {
                // gdcmData/LEADTOOLS_FLOWERS-16-MONO2-JpegLossless.dcm
                // has an empty 0008,0018 attribute, let's try to handle creating new UID
                anonymizedUID = uid.Generate();
            }

            copy.SetByteValue(anonymizedUID.c_str(), anonymizedUID.size());
            ds.Replace(copy);
        }
        else
        {
            TagValueKey tvk;
            tvk.first = tag;

            assert(dummyMapNonUIDTags.count(tvk) == 0 || dummyMapNonUIDTags.count(tvk) == 1);
            if (dummyMapNonUIDTags.count(tvk) == 0)
            {
                const char *ret = DummyValueGenerator::Generate(tvk.second.c_str());
                if (ret)
                {
                    dummyMapNonUIDTags[tvk] = ret;
                }
                else
                    dummyMapNonUIDTags[tvk] = "";
            }

            std::string &v = dummyMapNonUIDTags[tvk];
            copy.SetByteValue(v.c_str(), v.size());
        }
        ds.Replace(copy);
    }
    else
    {
        //Empty(tag);
        DataElement copy = ds.GetDataElement(tag);
        copy.Empty();
        ds.Replace(copy);
    }
    return true;
}

void gdcmAnonymizerStarviewer::RecurseDataSet(DataSet & ds)
{
    if (ds.IsEmpty()) return;

    static const unsigned int deidSize = sizeof(Tag);
    static const unsigned int numDeIds = sizeof(BasicApplicationLevelConfidentialityProfileAttributes) / deidSize;
    static const Tag *start = BasicApplicationLevelConfidentialityProfileAttributes;
    static const Tag *end = start + numDeIds;

    static const Global &g = Global::GetInstance();
    static const Defs &defs = g.GetDefs();
    const IOD& iod = defs.GetIODFromFile(*F);

    for (const Tag *ptr = start; ptr != end; ++ptr)
    {
        const Tag& tag = *ptr;
        // FIXME Type 1 !
        if (ds.FindDataElement(tag))
        {
            BALCPProtect(ds, tag, iod);
        }
    }

    DataSet::ConstIterator it = ds.Begin();
    for (; it != ds.End(); /*++it*/)
    {
        assert(it != ds.End());
        DataElement de = *it; ++it;
        //const SequenceOfItems *sqi = de.GetSequenceOfItems();
        VR vr = DataSetHelper::ComputeVR(*F, ds, de.GetTag());
        SmartPointer<SequenceOfItems> sqi = 0;
        if (vr == VR::SQ)
        {
            sqi = de.GetValueAsSQ();
        }
        if (sqi)
        {
            de.SetValue(*sqi); // EXTREMELY IMPORTANT #2912092
            de.SetVLToUndefined();
            assert(sqi->IsUndefinedLength());
            //de.GetVL().SetToUndefined();
            //sqi->SetLengthToUndefined();
            unsigned int n = sqi->GetNumberOfItems();
            for (unsigned int i = 1; i <= n; ++i)
            {
                Item &item = sqi->GetItem(i);
                DataSet &nested = item.GetNestedDataSet();
                RecurseDataSet(nested);
            }
        }
        ds.Replace(de);
    }
}

//void Anonymizer::SetAESKey(AES const &aes)
//{
//  AESKey = aes;
//}
//
//const AES &Anonymizer::GetAESKey() const
//{
//  return AESKey;
//}

} // end namespace gdcm
