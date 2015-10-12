#include "iusingirq.h"
#include "../drivers/irqmanager.h"

IrqManager* IUsingIrq::pIrqManager;

void IUsingIrq::RegIRQ(DWORD irq, IUsingIrq* pobj, DWORD param) const
{
	pIrqManager->RegIRQ(irq, pobj, param);
}
void IUsingIrq::EnableIRQ(DWORD irq) const
{
	pIrqManager->EnableIRQ(irq);
}
void IUsingIrq::DisableIRQ(DWORD irq) const
{
	pIrqManager->DisableIRQ(irq);
}
