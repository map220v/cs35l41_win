# CS35l41 Codec Driver for Windows
This driver only works with cs35l41 codecs found in Xiaomi Pad 5.

# Bugs
CS35L41_AMP_GAIN_CTRL and CS35L41_AMP_DIG_VOL_CTRL has no effect when DSP firmware is loaded or active (works in linux)

DSP seem to require I2S clock to apply settings properly when calling cs35l41_dsp_configure or cs35l41_dsp_mbox_start