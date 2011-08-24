#include "index_builder.hpp"
#include "features_vector.hpp"

#include "../defines.hpp"

#include "../coding/file_container.hpp"


namespace indexer
{
  bool BuildIndexFromDatFile(string const & datFile, string const & tmpFile)
  {
    try
    {
      FilesContainerR readCont(datFile);
      FeaturesVector featuresVector(readCont);

      FilesContainerW writeCont(datFile, FileWriter::OP_APPEND);

      FileWriter writer = writeCont.GetWriter(INDEX_FILE_TAG);

      feature::DataHeader header;
      header.Load(readCont.GetReader(HEADER_FILE_TAG));

      BuildIndex(header.GetScale(header.GetScalesCount()-1) + 1, featuresVector, writer, tmpFile);
    }
    catch (Reader::Exception const & e)
    {
      LOG(LERROR, ("Error while reading file: ", e.what()));
      return false;
    }
    catch (Writer::Exception const & e)
    {
      LOG(LERROR, ("Error writing index file: ", e.what()));
    }

#ifdef DEBUG
    FilesContainerR readCont(datFile);
    FilesContainerR::ReaderT r = readCont.GetReader(HEADER_FILE_TAG);
    int64_t const base = ReadPrimitiveFromPos<int64_t>(r, 0);
    LOG(LINFO, ("OFFSET = ", base));
#endif

    return true;
  }
} // namespace indexer
