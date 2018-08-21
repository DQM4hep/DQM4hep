//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

// -- dqm4hep headers
#include "dqm4hep/DQMNet.h"

namespace dqm4hep {

  namespace net {

    const char NullBuffer::buffer[] = "\0";
    const size_t NullBuffer::size = 1;

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    RawBuffer::RawBuffer() {
      this->adopt(NullBuffer::buffer, NullBuffer::size);
    }

    //-------------------------------------------------------------------------------------------------

    const char *RawBuffer::begin() const {
      return m_pBuffer;
    }

    //-------------------------------------------------------------------------------------------------

    const char *RawBuffer::end() const {
      return m_pBuffer + m_size;
    }

    //-------------------------------------------------------------------------------------------------

    size_t RawBuffer::size() const {
      return m_size;
    }

    //-------------------------------------------------------------------------------------------------

    void RawBuffer::adopt(const char *buffer, size_t s) {
      m_pBuffer = buffer;
      m_size = s;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    const RawBuffer &BufferModel::raw() const {
      return m_rawBuffer;
    }

    //-------------------------------------------------------------------------------------------------

    void BufferModel::handle(const char *buffer, size_t s) {
      m_rawBuffer.adopt(buffer, s);
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    Buffer::Buffer() {
      this->adopt(NullBuffer::buffer, NullBuffer::size);
    }

    //-------------------------------------------------------------------------------------------------

    Buffer::Buffer(Buffer &&buffer) {
      m_model = std::move(buffer.m_model);
    }

    //-------------------------------------------------------------------------------------------------

    std::shared_ptr<BufferModel> Buffer::createModel() const {
      return std::make_shared<BufferModel>();
    }

    //-------------------------------------------------------------------------------------------------

    void Buffer::setModel(std::shared_ptr<BufferModel> m) {
      if (!m)
        return;
      m_model = m;
    }

    //-------------------------------------------------------------------------------------------------

    const char *Buffer::begin() const {
      return m_model->raw().begin();
    }

    //-------------------------------------------------------------------------------------------------

    const char *Buffer::end() const {
      return m_model->raw().end();
    }

    //-------------------------------------------------------------------------------------------------

    size_t Buffer::size() const {
      return m_model->raw().size();
    }

    //-------------------------------------------------------------------------------------------------

    void Buffer::adopt(const char *buffer, size_t s) {
      auto m = this->createModel();
      m->handle(buffer, s);
      this->setModel(m);
    }

    //-------------------------------------------------------------------------------------------------

    BufferModelPtr Buffer::model() const {
      return m_model;
    }
  }
}
