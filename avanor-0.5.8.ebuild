# Copyright 1999-2003 Gentoo Technologies, Inc.
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit games

DESCRIPTION="Avanor is a fantasy rogue-like role-playing game"
HOMEPAGE="http://avanor.sourceforge.net/"
SRC_URI="mirror://sourceforge/avanor/${P}-src.tar.bz2"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~amd64"

DEPEND=">=sys-libs/ncurses-5"

src_compile() {
	make DATA_DIR="${GAMES_DATADIR}/${PN}/" OPTFLAGS="${CXXFLAGS}" || die "make failed"
}

src_install() {
	exeinto ${GAMES_BINDIR}
	doexe avanor
	dodir ${GAMES_DATADIR}/${PN}
	dodir ${GAMES_DATADIR}/${PN}/manual
	cp manual/* ${D}/${GAMES_DATADIR}/${PN}/manual
	dodoc changes.txt
	dohtml manual/*
	echo "hiscore table is empty" > ${D}/${GAMES_DATADIR}/${PN}/avanor.hsc
	prepgamesdirs
	chmod -R g+w ${D}/${GAMES_DATADIR}/${PN}/avanor.hsc
}
