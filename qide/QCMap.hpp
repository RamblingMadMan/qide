#ifndef QIDE_QCMAP_HPP
#define QIDE_QCMAP_HPP 1

#include <optional>

#include <QVector3D>
#include <QObject>
#include <QVector>
#include <QHash>

#include "QCToken.hpp"

#include "qide/shapes.hpp"

class QCMap: public QObject{
	Q_OBJECT

	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(QVector<Entity> entities READ entities WRITE setEntities NOTIFY entitiesChanged)

	public:
		struct Face{
			QVector<QVector3D> points;
			QString materialName;
			qreal xOff, yOff;
			qreal rot;
			qreal xScale, yScale;
		};

		struct Brush{
			QVector<Face> faces;
		};

		struct Entity{
			QString className;
			QVector3D origin;
			QHash<QString, QString> keys;
			std::optional<Brush> brush;
		};

		explicit QCMap(const QString &filePath, QObject *parent = nullptr);
		explicit QCMap(QObject *parent = nullptr);

		const QString &title() const noexcept{ return m_title; }
		const QVector<Entity> &entities() const noexcept{ return m_ents; }

		void loadFile(const QString &filePath);

		void loadStr(const QString &str);

		void setTitle(const QString &title_){
			if(title_ == m_title) return;
			m_title = title_;
			emit titleChanged();
		}

		void setEntities(const QVector<Entity> &ents){
			m_ents = ents;
			emit entitiesChanged();
		}

	signals:
		void titleChanged();
		void entitiesChanged();

	protected:
		using TokIt = QVector<QCToken>::ConstIterator;

		// parseXXXX all (except Map) get passed the token AFTER the starting token
		// and return the token after the end
		TokIt parseMap(TokIt beg, TokIt end);
		TokIt parseEntity(TokIt beg, TokIt end);
		TokIt parseBrush(TokIt beg, TokIt end, Entity *ent);
		TokIt parseFace(TokIt beg, TokIt end, Face *ret);

	private:
		QString m_title;
		QVector<Entity> m_ents;
};

Q_DECLARE_METATYPE(QCMap::Entity)
Q_DECLARE_METATYPE(QCMap::Brush)
Q_DECLARE_METATYPE(QCMap::Face)

#endif // !QIDE_QCMAP_HPP
