#ifndef GPT_H
#define GPT_H

#include "device.h"
#include "ata.h"

int discover_gpt(struct ata_device *dev);

#endif /* !GPT_H */
