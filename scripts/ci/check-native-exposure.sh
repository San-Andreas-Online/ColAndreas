#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
INCLUDE_FILE="${ROOT_DIR}/Server/include/colandreas.inc"
SOURCE_FILE="${ROOT_DIR}/src/ColAndreas.cpp"

if [[ ! -f "${INCLUDE_FILE}" ]]; then
	echo "Missing include file: ${INCLUDE_FILE}" >&2
	exit 1
fi

if [[ ! -f "${SOURCE_FILE}" ]]; then
	echo "Missing source file: ${SOURCE_FILE}" >&2
	exit 1
fi

tmp_dir="$(mktemp -d)"
trap 'rm -rf "${tmp_dir}"' EXIT

include_natives="${tmp_dir}/include_natives.txt"
registered_natives="${tmp_dir}/registered_natives.txt"

grep -Eo '^native[[:space:]]+[A-Za-z_][A-Za-z0-9_]*[[:space:]]*\(' "${INCLUDE_FILE}" \
	| sed -E 's/^native[[:space:]]+([A-Za-z_][A-Za-z0-9_]*)[[:space:]]*\(.*/\1/' \
	| sort -u > "${include_natives}"

grep -Eo '\{[[:space:]]*"[A-Za-z_][A-Za-z0-9_]*"[[:space:]]*,[[:space:]]*ColAndreasNatives::[A-Za-z_][A-Za-z0-9_]*[[:space:]]*\}' "${SOURCE_FILE}" \
	| sed -E 's/.*"([A-Za-z_][A-Za-z0-9_]*)".*/\1/' \
	| sort -u > "${registered_natives}"

if [[ ! -s "${include_natives}" ]]; then
	echo "No natives found in include file: ${INCLUDE_FILE}" >&2
	exit 1
fi

if [[ ! -s "${registered_natives}" ]]; then
	echo "No registered natives found in source file: ${SOURCE_FILE}" >&2
	exit 1
fi

missing_in_source="$(comm -23 "${include_natives}" "${registered_natives}" || true)"
missing_in_include="$(comm -13 "${include_natives}" "${registered_natives}" || true)"

if [[ -n "${missing_in_source}" || -n "${missing_in_include}" ]]; then
	echo "Native exposure mismatch detected." >&2

	if [[ -n "${missing_in_source}" ]]; then
		echo "Declared in colandreas.inc but not registered in src/ColAndreas.cpp:" >&2
		echo "${missing_in_source}" >&2
	fi

	if [[ -n "${missing_in_include}" ]]; then
		echo "Registered in src/ColAndreas.cpp but missing from colandreas.inc:" >&2
		echo "${missing_in_include}" >&2
	fi

	exit 1
fi

native_count="$(wc -l < "${include_natives}" | tr -d ' ')"
echo "Native exposure check passed (${native_count} natives)."
