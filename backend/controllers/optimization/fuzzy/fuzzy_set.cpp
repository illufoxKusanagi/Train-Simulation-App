#include "fuzzy_set.h"

FuzzySet::FuzzySet(const QString &name) : m_name(name) {}

QString FuzzySet::name() const { return m_name; }
