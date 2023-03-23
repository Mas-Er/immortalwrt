	u32 trcd;
	
	u32 mcfg;
	u32 reg_ddr_type1;
	u32 reg_ddr_type2;
	
	u32 dram_type;
	u32 cr;

	const char * const dram_types[] = {
		"LPDDR2 S2",
		"LPDDR2 S4",
		"DDR3",
		"LPDDR3",
		"Unknown"
	};

	const char * const cr_types[] = {
		"1T",
		"2T"
	};


	tcl = readl(rdev->iomem  DDR_PCTL_TCL) & 0xf;
	tras = readl(rdev->iomem  DDR_PCTL_TRAS) & 0x3f;
	trp = readl(rdev->iomem  DDR_PCTL_TRP) & 0xf;
	trcd = readl(rdev->iomem  DDR_PCTL_TRCD) & 0xf;

	mcfg = readl(rdev->iomem  DDR_PCTL_MCFG);

	reg_ddr_type1 = (mcfg & MCFG_DDR_MASK) >> MCFG_DDR_SHIFT;
	reg_ddr_type2 = (mcfg & MCFG_LPDDR_MASK) >> MCFG_LPDDR_SHIFT;
	cr = MCFG_CR_2T_BIT(mcfg);

	switch (reg_ddr_type1) {
		case MCFG_LPDDR2_S2:
			dram_type = 0;
			break;
		case MCFG_LPDDR2_S4:
			dram_type = 1;
			break;
		case MCFG_DDR3:
			dram_type = reg_ddr_type2 == LPDDR3_EN ? 3 : 2;
			break;
		default:
			dram_type = 4;
			break;
	}

	dev_info(rdev->dev, 
		"memory type %s, timings (tCL, tRCD, tRP, tRAS): CL%d-%d-%d-%d command rate: %s (mcfg register: 0x%x)\n", 
		dram_types[dram_type], tcl, trcd, trp, tras, cr_types[cr], mcfg);

	return 0;

}

static int rk3229_dmc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	struct rk3229_dmc *data;
	struct device_node *node_grf;
	int ret;

	data = devm_kzalloc(dev, sizeof(struct rk3229_dmc), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	//mutex_init(&data->lock);

	data->dmc_clk = devm_clk_get(dev, "ddr_sclk");
	if (IS_ERR(data->dmc_clk)) {
		if (PTR_ERR(data->dmc_clk) == -EPROBE_DEFER)
			return -EPROBE_DEFER;

		dev_err(dev, "Cannot get the clk dmc_clk\n");
		return PTR_ERR(data->dmc_clk);
	}

	data->edev = devfreq_event_get_edev_by_phandle(dev, "devfreq-events", 0);
	if (IS_ERR(data->edev))
		return -EPROBE_DEFER;

	data->iomem = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(data->iomem)) {
		dev_err(dev, "fail to ioremap iomem\n");
		ret = PTR_ERR(data->iomem);
		return ret;
	}

	data->dev = dev;

	rk3328_dmc_print_info(data);

	node_grf = of_parse_phandle(np, "rockchip,grf", 0);
	if (node_grf) {

		data->dram_type = rk3229_get_dram_type(dev, node_grf, data);

		if (data->dram_type == LPDDR2) {
			dev_warn(dev, "detected LPDDR2 memory\n");
		} else if (data->dram_type == DDR2) {
			dev_warn(dev, "detected DDR2 memory\n");
		} else if (data->dram_type == DDR3) {
			dev_info(dev, "detected DDR3 memory\n");
		} else if (data->dram_type == LPDDR3) {
			dev_info(dev, "detected LPDDR3 memory\n");
		} else if (data->dram_type == DDR4) {
			dev_info(dev, "detected DDR4 memory\n");
		} else if (data->dram_type == LPDDR4) {
			dev_info(dev, "detected LPDDR4 memory\n");
		} else if (data->dram_type == UNUSED) {
			dev_info(dev, "memory type not detected\n");
		} else {
			dev_info(dev, "unknown memory type: 0x%x\n", data->dram_type);
		}

	} else {

		dev_warn(dev, "Cannot get rockchip,grf\n");
		data->dram_type = UNUSED;

	}

	if (data->dram_type == DDR3 ||
		data->dram_type == LPDDR3 ||
		data->dram_type == DDR4 ||
		data->dram_type == LPDDR4) {

		ret = devfreq_event_enable_edev(data->edev);
		if (ret < 0) {
			dev_err(dev, "failed to enable devfreq-event devices\n");
			return ret;
		}

		ret = rk3229_dmc_init(pdev, data);
		if (ret)
			return ret;



		ret = rk3229_devfreq_init(data);
		if (ret)
			return ret;

	} else {

		dev_warn(dev, "detected memory type does not support clock scaling\n");

	}

	platform_set_drvdata(pdev, data);

	return 0;

}

static int rk3229_dmc_remove(struct platform_device *pdev)
{
	struct rk3229_dmc *rdev = dev_get_drvdata(&pdev->dev);

	/*
	 * Before remove the opp table we need to unregister the opp notifier.
	 */
	rk3229_devfreq_fini(rdev);

	if (ddr_psci_param)
		iounmap(ddr_psci_param);

	if (rdev->iomem)
		iounmap(rdev->iomem);

	return 0;
}

static const struct of_device_id rk3229_dmc_of_match[] = {
	{ .compatible = "rockchip,rk3229-dmc" },
	{ },
};
MODULE_DEVICE_TABLE(of, rk3229_dmc_of_match);

static struct platform_driver rk3229_dmc_driver = {
	.probe	= rk3229_dmc_probe,
	.remove = rk3229_dmc_remove,
	.driver = {
		.name	= "rk3229-dmc",
		.pm	= &rk3229_dmc_pm,
		.of_match_table = rk3229_dmc_of_match,
	},
};
module_platform_driver(rk3229_dmc_driver);

#ifdef CONFIG_ARM
static __init int sip_firmware_init(void)
{
	struct arm_smccc_res res;

	/*
	 * OP-TEE works on kernel 3.10 and 4.4 and we have different sip
	 * implement. We should tell OP-TEE the current rockchip sip version.
	 */

	/*
	 *
	 * 	res = __invoke_sip_fn_smc(ROCKCHIP_SIP_SIP_VERSION, ROCKCHIP_SIP_IMPLEMENT_V2,
				  SECURE_REG_WR, 0);

		arm_smccc_smc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);
	*/

	arm_smccc_smc(ROCKCHIP_SIP_SIP_VERSION, ROCKCHIP_SIP_IMPLEMENT_V2, SECURE_REG_WR, 0, 0, 0, 0, 0, &res);

	if (res.a0)
		pr_err("%s: set rockchip sip version v2 failed\n", __func__);

	pr_notice("Rockchip SIP initialized, version 0x%lx\n", res.a1);

	return 0;
}
arch_initcall(sip_firmware_init);
#endif

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Lin Huang <hl@rock-chips.com>");
MODULE_DESCRIPTION("RK3229 dmcfreq driver with devfreq framework");