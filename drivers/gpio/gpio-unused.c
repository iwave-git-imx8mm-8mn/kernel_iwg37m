/*
 * Copyright (c) 2020 iWave Systems Technologies Pvt. Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
/*
 * @file gpio-unused.c
 *
 * @brief Simple driver to set the unused GPIOs as input
 *
 * @ingroup GPIO
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>

/* 
 * iw_gpio_probe - Probe method for the GPIO device.
 * @np: pointer to device tree node
 *
 * This function probes the Unused GPIOs in the device tree. It request GPIOs
 * as input. It returns 0, if all the GPIOs is requested as input
 * or a negative value if there is an error.
 */

static int iw_gpio_probe(struct platform_device *pdev)
{
       struct device_node *np = pdev->dev.of_node;
       int i,val,err,num_ctrl;
       unsigned *ctrl;
       
       /* Fill GPIO pin array */
       num_ctrl = of_gpio_count(np);
       if (num_ctrl <= 0) {
               dev_err(&pdev->dev, "gpios DT property empty / missing\n");
               return -ENODEV;
       }
       ctrl = devm_kzalloc(&pdev->dev, num_ctrl * sizeof(unsigned),
                       GFP_KERNEL);
       if (!ctrl)
               return -ENOMEM;
       for (i = 0; i < num_ctrl; i++) {

               val = of_get_gpio(np, i);
               if (val < 0)
                       return val;
               ctrl[i] = val;
       }

       for (i = 0; i < num_ctrl; i++) {
               err = devm_gpio_request(&pdev->dev, ctrl[i],
                               "Unused GPIOs as i/p");
               if (err)
                       return err;
               err = gpio_direction_input(ctrl[i]);
               if (err)
                       return err;
       }

       return 0;
}

static int iw_gpio_remove(struct platform_device *pdev)
{
       /* Platform not registerd return silently */
       return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id iwgpio_match[] = {
       {.compatible = "iwave,unused-gpios"},
       {}
};
MODULE_DEVICE_TABLE(of, iwgpio_match);
#else
#define iwgpio_match NULL
#endif

static struct platform_driver iwgpio_driver = {
       .driver = {
               .name   = "iw_gpio",
               .owner  = THIS_MODULE,
               .of_match_table = of_match_ptr(iwgpio_match),
       },
       .probe          = iw_gpio_probe,
       .remove         = iw_gpio_remove,
};

static int __init iwgpio_init(void)
{
       return platform_driver_register(&iwgpio_driver);
}
subsys_initcall(iwgpio_init);

static void __exit iwgpio_exit(void)
{
       platform_driver_unregister(&iwgpio_driver);
}
module_exit(iwgpio_exit);

MODULE_AUTHOR("iWave Systems Technologies Pvt.Ltd");
MODULE_DESCRIPTION("iWave unused GPIO Driver");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL v2");
