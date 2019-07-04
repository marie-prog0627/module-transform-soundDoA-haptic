/*
 * Driver for the PCM1808 codec
 *
 * Author:	Yasuyuki Onodera
 *		Copyright 2016
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/soc.h>

static struct snd_soc_dai_driver pcm1808_dai = {
	.name = "pcm1808-dai",
	.capture = {
		.channels_min = 1,
		.channels_max = 2,
//		.rates = SNDRV_PCM_RATE_8000_192000,
		.rates = SNDRV_PCM_RATE_48000|SNDRV_PCM_RATE_96000,
		.formats = SNDRV_PCM_FMTBIT_S16_LE |
			   SNDRV_PCM_FMTBIT_S24_LE |
			   SNDRV_PCM_FMTBIT_S32_LE
	},
};

static struct snd_soc_codec_driver soc_codec_dev_pcm1808;

static int pcm1808_probe(struct platform_device *pdev)
{
	return snd_soc_register_codec(&pdev->dev, 
		&soc_codec_dev_pcm1808, &pcm1808_dai, 1);
}

static int pcm1808_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static const struct of_device_id pcm1808_of_match[] = {
	{ .compatible = "ti,pcm1808", },
	{ }
};
MODULE_DEVICE_TABLE(of, pcm1808_of_match);

static struct platform_driver pcm1808_codec_driver = {
	.probe 		= pcm1808_probe,
	.remove 	= pcm1808_remove,
	.driver		= {
		.name	= "pcm1808-codec",
		.owner	= THIS_MODULE,
		.of_match_table = pcm1808_of_match,
	},
};

module_platform_driver(pcm1808_codec_driver);

MODULE_DESCRIPTION("ASoC PCM1808 codec driver");
MODULE_AUTHOR("Yasuyuki Onodera");
MODULE_LICENSE("GPL v2");

