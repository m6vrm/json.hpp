.PHONY: test
test: json_test jsontestsuite_test
	./json_test
	./jsontestsuite_test

clean:
	$(RM) json_test
	$(RM) jsontestsuite_test
	$(RM) example

.PHONY: format
format:
	clang-format -i json.hpp json_test.cpp jsontestsuite_test.cpp example.cpp

json_test: json_test.cpp json.hpp
	c++ -o $@ $< -Wall -Wextra -Wpedantic -std=c++20 -fsanitize=address,undefined -g3

jsontestsuite_test: jsontestsuite_test.cpp json.hpp
	c++ -o $@ $< -Wall -Wextra -Wpedantic -std=c++20 -fsanitize=address,undefined -g3

example: example.cpp json.hpp
	c++ -o $@ $< -Wall -Wextra -Wpedantic -std=c++20 -fsanitize=address,undefined -g3
