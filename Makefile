CXXFLAGS := -I3party -Ilibtree/include

.PHONY: all
all: bin shared

.PHONY: bin
bin:
	$(CXX) $(CXXFLAGS) -Os *.cpp

# .PHONY: shared
# shared:
# 	$(CXX) $(CXXFLAGS) -c -Os -fPIC *.cpp
# 	$(CXX) *.o -shared -o libtree.so

.PHONY: clean
clean:
	rm -rf ./*.o

.PHONY: fullclean
fullclean: clean
	rm -rf *.out *.so

.PHONY: sizes
sizes:
	ls -lah | grep 'libtree.so'
	ls -lah | grep 'a.out'
