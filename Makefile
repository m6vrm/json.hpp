.PHONY: test
test: jsontestsuite_test json_test
	./jsontestsuite_test
	./json_test

.PHONY: clean
clean:
	$(RM) jsontestsuite_test
	$(RM) json_test
	$(RM) example

.PHONY: format
format:
	clang-format -i json.hpp json_test.cpp jsontestsuite_test.cpp example.cpp

json_test: json_test.cpp json.hpp
	c++ -o $@ $< -std=c++20 -Wall -Wextra -Wpedantic -g3 -fsanitize=address,undefined

jsontestsuite_test: jsontestsuite_test.cpp json.hpp
	c++ -o $@ $< -std=c++20 -Wall -Wextra -Wpedantic -g3 -fsanitize=address,undefined

example: example.cpp json.hpp
	c++ -o $@ $< -std=c++20 -Wall -Wextra -Wpedantic -g3 -fsanitize=address,undefined
