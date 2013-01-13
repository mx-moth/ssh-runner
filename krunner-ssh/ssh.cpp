/*
 *   Copyright (C) 2009 Edward "Hades" Toroshchin <kde@hades.name>
 *   Copyright (C) 2008 Sun Microsystems, Inc.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QMutexLocker>
#include <QProcess>

#include <KAction>
#include <KDebug>
#include <KIcon>
#include <KRun>

#include <iostream>

#include "ssh.h"

struct SSHHost
{
	QString name;
};

class SSHConfigReader
{
	QList< SSHHost > *list;
	QDateTime lastChecked;
	QMutex mutex;

	QString sshdir;
public:
	SSHConfigReader() : list(0) {
		sshdir = QString(QDir::home().filePath(".ssh"));
	}

	~SSHConfigReader() {
		if( list ) delete list;
		list = 0;
	}

	void updateAsNeccessary() {

		QMutexLocker _ml(&mutex);
		QDir dir(sshdir);

		QString config_path = dir.filePath("config");

		if (list && lastChecked >= QFileInfo(config_path).lastModified()) {
			return;
		}

		if (list) delete list;

		list = new QList<SSHHost>;

		QFile config(config_path);
		if (!config.open(QIODevice::ReadOnly)) {
			return;
		}

		QTextStream stream(&config);
		stream.setCodec("UTF-8");

		while (!stream.atEnd()) {
			QString line = stream.readLine();

			line = line.trimmed();
			if (line.isEmpty()) {
				continue;
			}

			if (line.startsWith("host ", Qt::CaseInsensitive)) {

				QString hostname = line.mid(5).trimmed();

				SSHHost host;
				host.name = hostname;

				(*list) << host;
			}
		}

		config.close();

		lastChecked = QDateTime::currentDateTime();

	}

	QList<SSHHost> hosts() {
		updateAsNeccessary();
		if (!list) return QList<SSHHost>();

		return *list;
	}
};

SSHRunner::SSHRunner(QObject *parent, const QVariantList& args) : Plasma::AbstractRunner(parent, args), rd( 0 ) {
	mIcon = KIcon("utilities-terminal");
	rd = new SSHConfigReader;
	setObjectName("SSH Host runner");
	setSpeed(AbstractRunner::SlowSpeed);
}

SSHRunner::~SSHRunner() {
	if(rd) delete rd;
	rd = 0;
}

Plasma::QueryMatch SSHRunner::constructMatch(QString host, Plasma::QueryMatch::Type priority) {
	Plasma::QueryMatch match(this);
	match.setText(QString("SSH to host %1").arg(host));
	match.setType(priority);
	match.setIcon(mIcon);
	match.setData(QVariant(host));
	return match;
}

void SSHRunner::match(Plasma::RunnerContext &context) {
	QString request = context.query();

	bool startsWithSSH = false;
	bool exactMatchFound = false;
	if (request.startsWith("ssh ", Qt::CaseInsensitive)) {
		request.remove(0, 4);
		startsWithSSH = true;
	}

	if (request.isEmpty())
		return;

	QList<Plasma::QueryMatch> matches;
	foreach(SSHHost h, rd->hosts()) {
		if (h.name.compare(request, Qt::CaseInsensitive) == 0) {
			matches << constructMatch(h.name, Plasma::QueryMatch::ExactMatch);
			exactMatchFound = true;
		} else if (request.length() > 2) {
			if (h.name.startsWith(request, Qt::CaseInsensitive)) {
				matches << constructMatch(h.name, Plasma::QueryMatch::PossibleMatch);
			} else if (h.name.contains(request, Qt::CaseInsensitive)) {
				matches << constructMatch(h.name, Plasma::QueryMatch::CompletionMatch);
			}
		}
	}

	if (startsWithSSH && !exactMatchFound) {
		Plasma::QueryMatch match(this);

		match.setType(Plasma::QueryMatch::CompletionMatch);

		match.setText(QString("SSH to %1").arg(request));
		match.setIcon(mIcon);
		match.setData(QVariant(request));

		matches << match;
	}

	context.addMatches(context.query(), matches);
}

void SSHRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
	Q_UNUSED(context);

	QString host = match.data().toString();
	KRun::shellQuote(host);

	QString command = QString("ssh %1").arg(host);

	QString konsole_command = QString("konsole -e %1").arg(command);

	KRun::runCommand(konsole_command, 0);
}

bool SSHRunner::isRunning(const QString name) {
	Q_UNUSED(name);
	// TODO Work out if there is an active connection to a host
	return false;
}

QList<QAction*> SSHRunner::actionsForMatch(const Plasma::QueryMatch &match) {
	Q_UNUSED(match);

	QList<QAction*> ret;

	if(!action("ssh")) {
		(addAction("ssh", mIcon, i18n("SSH to remote host")))->setData("ssh");
	}

	ret << action("ssh");
	return ret;
}

