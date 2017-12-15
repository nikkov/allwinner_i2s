/*
 * Driver for clocks board for i2s bus
 *
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
#include <linux/of_device.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#if (1)
#define DBGOUT(msg...)		do { printk(KERN_ERR msg); } while (0)
#else
#define DBGOUT(msg...)		do {} while (0)
#endif


struct i2s_clock_board_priv {
	unsigned long clk_gpios[3];
};

static int i2s_clock_board_soc_probe(struct snd_soc_codec *codec)
{
	struct i2s_clock_board_priv *priv = snd_soc_codec_get_drvdata(codec);
	DBGOUT("i2s_clock_board: %s\n", __func__);
	return 0;
}

static int i2s_clock_board_soc_remove(struct snd_soc_codec *codec)
{
	struct i2s_clock_board_priv *priv = snd_soc_codec_get_drvdata(codec);
	DBGOUT("i2s_clock_board: %s\n", __func__);
	return 0;
}

static int i2s_clock_board_soc_suspend(struct snd_soc_codec *codec)
{
	struct i2s_clock_board_priv *priv = snd_soc_codec_get_drvdata(codec);
	DBGOUT("i2s_clock_board: %s\n", __func__);
	return 0;
}

static int i2s_clock_board_soc_resume(struct snd_soc_codec *codec)
{
	struct i2s_clock_board_priv *priv = snd_soc_codec_get_drvdata(codec);
	DBGOUT("i2s_clock_board: %s\n", __func__);
	return 0;
}

static const struct snd_soc_codec_driver soc_codec_i2s_clock_board = {
	.probe = i2s_clock_board_soc_probe,
	.remove = i2s_clock_board_soc_remove,
	.suspend = i2s_clock_board_soc_suspend,
	.resume = i2s_clock_board_soc_resume,
};

static int i2s_clock_board_set_dai_fmt(struct snd_soc_dai *codec_dai,
			      unsigned int format)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	DBGOUT("i2s_clock_board: %s\n", __func__);
	return 0;
}


static int set_clock(struct i2s_clock_board_priv *priv, unsigned long rate)
{
	int clk_family = 1;
	u8 div_tb[2] = { 0, 0 };

	switch(rate)
	{
		case 44100:
			clk_family = 0;
		case 48000:
			div_tb[0] = 1;
			div_tb[1] = 1;
			break;
		case 88200:
			clk_family = 0;
		case 96000:
			div_tb[0] = 1;
			div_tb[1] = 0;
			break;
		case 176400:
			clk_family = 0;
		case 192000:
			div_tb[0] = 0;
			div_tb[1] = 0;
			break;
		default:
			return ENOENT;
	}

	DBGOUT("%s: clk_gpios[%d] = %d\n", __func__, priv->clk_gpios[0], clk_family);
	DBGOUT("%s: clk_gpios[%d] = %d\n", __func__, priv->clk_gpios[1], div_tb[0]);
	DBGOUT("%s: clk_gpios[%d] = %d\n", __func__, priv->clk_gpios[2], div_tb[1]);

	gpio_set_value(priv->clk_gpios[0], clk_family);
	gpio_set_value(priv->clk_gpios[1], div_tb[0]);
	gpio_set_value(priv->clk_gpios[2], div_tb[1]);
/*
	gpio_direction_output(priv->clk_gpios[0], clk_family);
	gpio_direction_output(priv->clk_gpios[1], div_tb[0]);
	gpio_direction_output(priv->clk_gpios[2], div_tb[1]);
*/	
	return 0;
}


static int i2s_clock_board_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct i2s_clock_board_priv *priv = snd_soc_codec_get_drvdata(codec);
	DBGOUT("i2s_clock_board: %s, physical_width=%d, rate=%d, width=%d\n", __func__, 
		(int)params_physical_width(params),
		(int)params_rate(params),
		(int)params_width(params)
	);
	return set_clock(priv, params_rate(params));
}

static int i2s_clock_board_hw_free(struct snd_pcm_substream *substream,
			  struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct i2s_clock_board_priv *priv = snd_soc_codec_get_drvdata(codec);
	DBGOUT("i2s_clock_board: %s\n", __func__);
	return 0;
}

static const struct snd_soc_dai_ops i2s_clock_board_dai_ops = {
	.set_fmt	= i2s_clock_board_set_dai_fmt,
	.hw_params	= i2s_clock_board_hw_params,
	.hw_free	= i2s_clock_board_hw_free,
};

#define SNDRV_PCM_RATE_44100_192000 (SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000 | \
		SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 | \
		SNDRV_PCM_RATE_192000)

static struct snd_soc_dai_driver i2s_clock_board_dai = {
	.name		= "i2s_clock_board",
	.playback = {
		.stream_name	= "Playback",
		.channels_min	= 2,
		.channels_max	= 2,
		.rates		= SNDRV_PCM_RATE_44100_192000,
		.formats	= SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE,
	},
	.capture = {
		.stream_name	= "Capture",
		.channels_min	= 2,
		.channels_max	= 2,
		.rates		= SNDRV_PCM_RATE_44100_192000,
		.formats	= SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE,
	},
	.ops	= &i2s_clock_board_dai_ops,
};

static const struct of_device_id i2s_clock_board_dt_ids[] = {
	{ .compatible = "custom,i2s_clock_board", },
	{ }
};
MODULE_DEVICE_TABLE(of, i2s_clock_board_dt_ids);

static int i2s_clock_board_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct i2s_clock_board_priv *priv;
	int ret, i;
	DBGOUT("i2s_clock_board: %s\n", __func__);

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
		
	dev_set_drvdata(dev, priv);
	for (i = 0; i < ARRAY_SIZE(priv->clk_gpios); i++) {
		ret = of_get_named_gpio(dev->of_node, "clk-gpios", i);
		if (ret < 0) {
			dev_err(dev, "invalid clk-gpios property\n");
			return ret;
		}
		priv->clk_gpios[i] = ret;
		DBGOUT("%s: priv->clk_gpios[%d] = %d\n", __func__, i, ret);
	}		
	return snd_soc_register_codec(dev, &soc_codec_i2s_clock_board,
				      &i2s_clock_board_dai, 1);
}

static int i2s_clock_board_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static struct platform_driver i2s_clock_board_driver = {
	.probe		= i2s_clock_board_probe,
	.remove		= i2s_clock_board_remove,
	.driver		= {
		.name	= "i2s_clock_board",
		.of_match_table = of_match_ptr(i2s_clock_board_dt_ids),
	},
};

module_platform_driver(i2s_clock_board_driver);

MODULE_DESCRIPTION("ASoC external clock board for I2S bus");
MODULE_LICENSE("GPL v2");
