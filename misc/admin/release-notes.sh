#!/bin/bash

# This script generates the release notes every time a release is made.
#
# Usage example:
# sh release-notes.sh 3.5.0

# Based very heavily on Curtis Rueden's equivalent in https://github.com/maven-nar/nar-maven-plugin, released under Apache 2.0.

OWNER=susanw1
PROJECT_NAME=zscript
GITHUB_URL="https://github.com/${OWNER}/${PROJECT_NAME}"

PROJECT_DESC="Zscript"

# -- Functions --

githubIssue() {
	page=$(curl -fsL "${GITHUB_URL}/issues/$1")
	if echo "$page" | grep -q 'State State--\(red\|purple\)'
	then
		# Issue is closed or merged.
		echo "$page" | grep -A 1 'js-issue-title' | tail -n1 | sed 's/^ *//'
	else
		# Issue was not fixed!
		echo
	fi
}

# -- Main --

v=$1
if [ -z "$v" ]
then
  echo "Usage: sh release-notes.sh <new-version>"
  exit 1
fi

git fetch --tags
p=$(git tag -l | grep -B 1 "${PROJECT_NAME}-$v" | head -n1 | sed "s/.*${PROJECT_NAME}-//")
if [ -z "$p" ]
then
  echo "Invalid version: $v"
  exit 2
fi
echo "Version  = $v"
echo "Previous = $p"

echo
echo "--------------------------------"
echo

echo "It is a great honor to announce a new version of ${PROJECT_DESC}."

echo
echo "${PROJECT_NAME}-$v:"
echo "======================="
echo "$(git log -1 "${PROJECT_NAME}-$v"  --pretty=format:"%ad")"

echo
echo "authors with commit counts"
echo "--------------------------"
echo
echo "Author | Count"
echo "------:|:-----"
git shortlog -ns "${PROJECT_NAME}-$p..${PROJECT_NAME}-$v" | while read line
do
	commits=${line%	*}
	author=${line#*	}
	echo "$author | $commits"
done

echo
echo "list of commits"
echo "--------------------"
echo
echo "Author | Commit"
echo "------:|:------"
git log "${PROJECT_NAME}-$p..${PROJECT_NAME}-$v" --pretty=format:"%an | %s" | grep -v 'Bump to next development cycle' | grep -v 'prepare release' | grep -v 'Merge pull request'

echo
echo "issues fixed"
echo "---------------"
echo
echo "Issue | Title"
echo "-----:|:-----"
git log "${PROJECT_NAME}-$p..${PROJECT_NAME}-$v" --pretty=format:"%an|%s|%B" | grep '#[0-9][0-9]*' | sed 's/.*#\([0-9][0-9]*\).*/\1/' | sort -nu | while read issue
do
	title=$(githubIssue "$issue")
	test "$title" && echo "#$issue | $title"
done
