#ifndef QCPROGS_HPP
#define QCPROGS_HPP

#include <QObject>
#include <QMap>

class QCParser;

class QCProgs : public QObject
{
	Q_OBJECT

	public:
		explicit QCProgs(QObject *parent = nullptr);

		QCParser *insert(const QString &name);

		QCParser *get(const QString &name);

		template<typename Fn>
		QCParser *getOr(const QString &name, Fn &&fn){
			auto res = get(name);
			if(!res){
				res = insert(name);
				std::forward<Fn>(fn)(res);
			}

			return res;
		}

		QList<QCParser*> parsers() const{ return m_parsers.values(); }

	private:
		QMap<QString, QCParser*> m_parsers;
};

#endif // QCPROGS_HPP
