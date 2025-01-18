include(FetchContent)

# For whatever reason using git clone here didn't work so we grab tar ball instead
FetchContent_Declare(
	uv
	URL https://github.com/libuv/libuv/archive/refs/tags/v1.50.0.tar.gz
	URL_HASH SHA256=b1ec56444ee3f1e10c8bd3eed16ba47016ed0b94fe42137435aaf2e0bd574579
)

FetchContent_Declare(
	jansson
	GIT_REPOSITORY https://github.com/akheron/jansson
	GIT_TAG 61fc3d0e28e1a35410af42e329cd977095ec32d2 
)

FetchContent_MakeAvailable(
	uv
	jansson
)

set(CABOR_EXTERNAL_LIBRARIES ${CABOR_EXTERNAL_LIBRARIES}
	uv
	jansson
PARENT_SCOPE)

set(CABOR_EXTERNAL_INCLUDE_DIRECTORIES ${CABOR_INCLUDE_DIRECTORIES}
	${uv_SOURCE_DIR}/include
	${jansson_BINARY_DIR}/include
PARENT_SCOPE)
