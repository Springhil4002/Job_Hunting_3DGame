#include "Tags.h"

std::set<std::string>::const_iterator Tags::begin() const
{
	return m_tags.begin();
}

std::set<std::string>::const_iterator Tags::end() const
{
	return m_tags.end();
}

void Tags::AddTag(const std::string _tag)
{
	m_tags.insert(_tag);
}

void Tags::RemoveTag(const std::string _tag)
{
	m_tags.erase(_tag);
}

bool Tags::SearchTag(const std::string _tag) const
{
	return m_tags.count(_tag);
}

void Tags::ClearTags()
{
	m_tags.clear();
}
