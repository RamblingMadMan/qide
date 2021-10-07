#ifndef QIDE_QIDEVMDOCK_HPP
#define QIDE_QIDEVMDOCK_HPP 1

#include <QWidget>

class QCVM;

class QideVMDock: public QWidget{
	Q_OBJECT

	public:
		explicit QideVMDock(QCVM *vm_ = nullptr, QWidget *parent = nullptr);

		QCVM *vm() const noexcept{ return m_vm; }
		QStringList fns() const noexcept;
		QStringList vars() const noexcept;
		QStringList fields() const noexcept;

		void setVm(QCVM *vm_);

	signals:
		void vmChanged();
		void fnsChanged();
		void varsChanged();
		void fieldsChanged();

	private:
		void updateData();

		QCVM *m_vm;
		QStringList m_fns, m_vars, m_fields;
};

#endif // !QIDE_QIDEVMDOCK_HPP
