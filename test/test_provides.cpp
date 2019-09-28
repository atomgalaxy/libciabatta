#include "ciabatta/ciabatta.hpp"

#include <utility>

struct message { /* payload */
};
struct abstract_socket {
  virtual void receive(message) = 0;
  virtual void send(message) = 0;
};

template <typename Base>
struct null_sender : Base {
  CIABATTA_DEFAULT_MIXIN_CTOR(null_sender, Base);
  void send(message m) final {}
};

template <typename Base>
struct null_receiver : Base {
  CIABATTA_DEFAULT_MIXIN_CTOR(null_receiver, Base);
  void receive(message m) final {}
};

template <typename Base>
struct is_socket : Base, abstract_socket {
  CIABATTA_DEFAULT_MIXIN_CTOR(is_socket, Base);
};

struct null_socket
    : ciabatta::mixin<null_socket, null_sender, null_receiver, is_socket> {};

struct null_socket2
    : ciabatta::mixin<null_socket2,
                      null_sender,
                      null_receiver,
                      ciabatta::mixins::provides<abstract_socket>::mixin> {};

struct null_socket3
    : ciabatta::mixin<
          null_socket3,
          null_sender,
          null_receiver,
          ciabatta::curry<ciabatta::mixins::provides, abstract_socket>::mixin> {
};

int main() {
  null_socket s1;
  abstract_socket& s1_ref = s1;
  s1_ref.send(message{});
  s1_ref.receive(message{});

  null_socket2 s2;
  abstract_socket& s2_ref = s2;
  s2_ref.send(message{});
  s2_ref.receive(message{});

  null_socket3 s3;
  abstract_socket& s3_ref = s3;
  s3_ref.send(message{});
  s3_ref.receive(message{});
}
