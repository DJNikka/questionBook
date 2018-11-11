#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class [[eosio::contract]] questionbook : public eosio::contract {

public:
  using contract::contract;
  
  questionbook(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

  [[eosio::action]]
  void upsert(name user, std::string full_question, std::string answer, std::string citation) {
    require_auth( user );
    questionbook_type questions(_code, _code.value);
    auto iterator = questions.find(user.value);
    if( iterator == questions.end() )
    {
      questions.emplace(user, [&]( auto& row ) {
       row.key = user;
       row.full_question = full_question;
       row.answer = answer;
       row.citation = citation;
      
      });
    }
    else {
      std::string changes;
      questions.modify(iterator, user, [&]( auto& row ) {
        row.key = user;
        row.full_question = full_question;
        row.answer = answer;
        row.citation = citation;
        
      });
    }
  }

  [[eosio::action]]
  void erase(name user) {
    require_auth(user);

    questionbook_type questions(_self, _code.value);

    auto iterator = questions.find(user.value);
    eosio_assert(iterator != questions.end(), "Record does not exist");
    questions.erase(iterator);
  }

private:
  struct [[eosio::table]] prompt {
    name key;
    std::string full_question;
    std::string answer;
    std::string citation;
    uint64_t primary_key() const { return key.value; }

  };
  typedef eosio::multi_index<"prompts"_n, prompt> questionbook_type;

};

EOSIO_DISPATCH( questionbook, (upsert)(erase))