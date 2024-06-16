MAKEFLAGS += --no-print-directory	# Do not print "Entering directory ..."

### Targets ###

# Clean up
.PHONY: clean
clean:
	rm -rf build || true

# Build
.PHONY: build
build:
	mkdir -p build
	cd build && cmake .. && make

# Tests
.PHONY: tests
tests: build
	./build/tests/tests

# Flash
.PHONY: flash
flash: build
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
	-c "init; halt; stm32f1x mass_erase 0; flash write_image erase build/firmware/game_of_life_fw.bin 0x08000000; reset run; exit"

# Restart the board.
.PHONY: restart
restart:
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "init; reset halt; reset run; exit"

# Run pre-commit (lint).
.PHONY: pre-commit
pre-commit:
ifdef CI
	pre-commit run --all-files
else
	export PRE_COMMIT_HOME=$$(pwd)/.cache/pre-commit && pre-commit run --all-files
endif

# Clang-tidy
.PHONY: clang-tidy
clang-tidy: build
	find . -name "*.cpp" -o -name "*.hpp" | grep -v build | grep -v .cache | xargs clang-tidy

### Docker ###

# Commands below can be helpful when workinbg outside the dev container.
IMAGE_BASE_NAME=game-of-life-env

ifdef CI
IMAGE=$(IMAGE_BASE_NAME):$$(git rev-parse --short HEAD)
DOCKER_RUN_ARGS=
else
IMAGE=$(IMAGE_BASE_NAME)
DOCKER_RUN_ARGS="-it -v /dev/:/dev --privileged"
endif

CMD ?= ""

# Build the Docker image.
.PHONY: docker
docker:
	docker build . \
	--build-arg USER_UID=$(shell id -u) \
	--build-arg USER_GID=$(shell id -g) \
	-t $(IMAGE) -f .devcontainer/Dockerfile

# Run the container and execute an optional command.
# For example: make container CMD="make build"
.PHONY: container
container: docker
	eval docker run --rm $(DOCKER_RUN_ARGS) -v $$PWD:/data $(IMAGE) $(CMD)

# Specifies command aliases to be run from the Docker container.
# For example, `make container-build` is equivalent to `make container CMD="make build"`.
RUN_TARGETS = build clean tests flash pre-commit clang-tidy

.PHONY: $(RUN_TARGETS) $(addprefix container-, $(RUN_TARGETS))

$(addprefix container-, $(RUN_TARGETS)):
	@$(MAKE) container CMD="make $(subst container-,,$@)"
