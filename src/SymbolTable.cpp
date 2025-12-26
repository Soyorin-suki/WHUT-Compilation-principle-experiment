#include "SymbolTable.hpp"

SymbolTable::SymbolTable() {
	reset();
}

void SymbolTable::reset() {
	scopes.clear();
	enterScope(); // global
}

void SymbolTable::enterScope() {
	scopes.emplace_back();
}

void SymbolTable::leaveScope() {
	if (scopes.empty()) {
		return;
	}
	scopes.pop_back();
}

bool SymbolTable::declare(const std::string& name, const Symbol& sym) {
	if (scopes.empty()) {
		enterScope();
	}
	auto& table = scopes.back().table;
	if (table.find(name) != table.end()) {
		return false;
	}
	table.emplace(name, sym);
	return true;
}

const Symbol* SymbolTable::lookupCurrent(const std::string& name) const {
	if (scopes.empty()) {
		return nullptr;
	}
	const auto& table = scopes.back().table;
	auto it = table.find(name);
	if (it == table.end()) {
		return nullptr;
	}
	return &it->second;
}

const Symbol* SymbolTable::lookup(const std::string& name) const {
	for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
		auto found = it->table.find(name);
		if (found != it->table.end()) {
			return &found->second;
		}
	}
	return nullptr;
}