#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <boost/array.hpp>

#include <boost/python.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/str.hpp>
#include <boost/python/tuple.hpp>

#include <fc/io/json.hpp>
#include <fc/crypto/hex.hpp>
#include <fc/crypto/ripemd160.hpp>

#include <fc/smart_ref_fwd.hpp>
#include <fc/smart_ref_impl.hpp>
#include <fc/bitutil.hpp>
#include <fc/variant.hpp>
#include <fc/exception/exception.hpp>

#include <fc/crypto/elliptic.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <fc/crypto/sha512.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/crypto/digest.hpp>

#include <eos/types/types.hpp>

#include <eos/chain/types.hpp>
#include <eos/chain/transaction.hpp>
#include <eos/chain/block.hpp>

#include <eos/chain/type_object.hpp>

#include <eos/utilities/key_conversion.hpp>

using namespace boost;
using namespace eos::utilities;
using namespace eos::chain;
using namespace boost::python;

void hack_() {
}

std::string eoshelper_Transfer(const std::string& from, const std::string& to, long amount, const std::string& memo) {
  eos::types::Transfer transfer;

  transfer.from   = from;
  transfer.to     = to;
  transfer.amount = Asset(amount);
  transfer.memo   = memo;

  return fc::to_hex(fc::raw::pack(transfer));
}

std::string eoshelper_SetCode(const std::string& account, int vmtype, int vmversion, boost::python::object py_buffer ) {

  eos::types::SetCode set_code;
  
  set_code.account   = account;
  set_code.vmtype    = vmtype;
  set_code.vmversion = vmversion;

  boost::python::object locals(python::borrowed(PyEval_GetLocals()));
  boost::python::object py_iter = locals["__builtins__"].attr("iter");
  boost::python::stl_input_iterator<char> begin( py_iter(py_buffer)), end;

  // Copy the py_buffer into a local buffer with known continguous memory.
  set_code.code = eos::types::Bytes(begin, end);

  return fc::to_hex(fc::raw::pack(set_code));
}

std::string eoshelper_DefineStruct(const std::string& scope, const std::string& struct_name, const std::string& struct_base, const boost::python::list& fields) {
  
  eos::types::DefineStruct define_struct;

  eos::types::Struct _struct;
  _struct.name = struct_name;
  _struct.base = struct_base;

  std::cout << boost::python::len(fields) << std::endl;

  for(int i=0; i<boost::python::len(fields); i++) {
    
    eos::types::Field field;
    
    auto t = boost::python::extract<boost::python::tuple>(fields[i])();

    field.name = boost::python::extract<const char*>(t[0])();
    field.type = boost::python::extract<const char*>(t[1])();
    
    _struct.fields.push_back(field);
  }

  define_struct.scope      = scope;
  define_struct.definition = _struct;

  return fc::to_hex(fc::raw::pack(define_struct));
}

std::string eoshelper_priv_to_pubkey(const std::string& priv_hex) {
  auto pk = fc::ecc::private_key::regenerate(fc::sha256(priv_hex));
  public_key_type pub = pk.get_public_key();
  return std::string(pub);
}

std::string eoshelper_sign_compact(const std::string& digest, const std::string& wif) {

  auto pk = wif_to_key(wif);
  auto tmp = fc::sha256(digest);
  auto sig = pk->sign_compact(tmp);

  return fc::to_hex(fc::raw::pack(sig));
}

std::string eoshelper_tx_sign(const std::string& tx_json, const std::string& wif, const std::string& chain_id) {

  auto pk = wif_to_key(wif);
  
  auto tx = fc::json::from_string(tx_json).as<eos::chain::SignedTransaction>();

  const signature_type& sig = tx.sign(*pk, fc::sha256(chain_id));

  return fc::json::to_string(tx);
}

boost::python::tuple eoshelper_tapos_info(const std::string& block_id) 
{
  auto h = fc::sha256(block_id);

  return boost::python::make_tuple( 
    (uint16_t)fc::endian_reverse_u32(h._hash[0]), //refBlockNum
    (uint32_t)h._hash[1]                          //refBlockPrefix
  );
}

std::string eoshelper_block_id(const std::string& signed_block_header_json) {

  auto block_header = fc::json::from_string(signed_block_header_json).as<eos::chain::signed_block_header>();
  auto id           = block_header.id();
  auto data         = fc::raw::pack(id);

  return fc::to_hex(data);
}

std::string eoshelper_tx_id(const std::string& tx_json) {

  auto tx   = fc::json::from_string(tx_json).as<eos::chain::SignedTransaction>();
  auto id   = tx.id();
  auto data = fc::raw::pack(id);

  return fc::to_hex(data);
}

void translate(fc::exception const& e)
{
  //Use the Python 'C' API to set up an exception object
  PyErr_SetString(PyExc_RuntimeError, e.to_detail_string().c_str());
}
 
BOOST_PYTHON_MODULE(eoshelper)
{
  register_exception_translator<fc::exception>(&translate);

  def("eoshelper_priv_to_pubkey",     eoshelper_priv_to_pubkey);
  def("eoshelper_tapos_info",         eoshelper_tapos_info);
  def("eoshelper_tx_sign",            eoshelper_tx_sign);
  def("eoshelper_tx_id",              eoshelper_tx_id);
  def("eoshelper_block_id",           eoshelper_block_id);
  def("eoshelper_sign_compact",       eoshelper_sign_compact);
  def("eoshelper_Transfer",           eoshelper_Transfer);
  def("eoshelper_DefineStruct",       eoshelper_DefineStruct);
  def("eoshelper_SetCode",            eoshelper_SetCode);
}