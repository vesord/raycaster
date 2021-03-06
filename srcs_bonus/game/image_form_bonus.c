/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_form_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/17 12:54:42 by matrus            #+#    #+#             */
/*   Updated: 2020/08/02 10:28:34 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"
#include "includes/cub_game_bonus.h"
#include "includes/cub_textures_bonus.h"

int			process_game(t_cub *cub)
{
	t_img *tmp_frm;

	if (!cub->frm_0
		&& (!(cub->frm_0 = frame_init(cub->win->mlx_ptr, cub->win->x,
						cub->win->y))
		|| !(cub->frm_1 = frame_init(cub->win->mlx_ptr, cub->win->x,
						cub->win->y))))
		cub_destroy(cub, ERR_NO_MEMORY);
	if (cub->hud->status)
		set_status_screen(cub);
	else
	{
		process_key(cub);
		check_interactions(cub);
		update_hud(cub);
		make_frame(cub);
	}
	mlx_put_image_to_window(cub->win->mlx_ptr, cub->win->win_ptr,
						cub->frm_0->ptr, 0, 0);
	tmp_frm = cub->frm_0;
	cub->frm_0 = cub->frm_1;
	cub->frm_1 = tmp_frm;
	return (0);
}

void		make_frame(t_cub *cub)
{
	int		frame_x;
	float	angle;
	float	d_angle;

	angle = cub->cam->cam_direction_yaw - cub->cam->cam_angle_yaw / 2;
	d_angle = cub->cam->cam_angle_yaw / cub->win->x;
	frame_x = 0;
	while (frame_x < cub->win->x)
	{
		frame_col_set(frame_x, throw_ray(cub, angle,
			cub->cam->cam_direction_yaw), cub);
		angle += d_angle;
		frame_x++;
	}
}

void		frame_col_set(int f_x, float len_to_wall, t_cub *cub)
{
	int			f_y;
	t_frm_col	col;

	col = frame_s_col_init(len_to_wall, cub);
	f_y = -1;
	while (++f_y < cub->win->y)
	{
		if ((((unsigned int*)cub->frm_0->data)[f_y * cub->win->x + f_x]
		& 0xff000000) == HUD_TRANSP_MASK)
			;
		else if (col.angle > col.c_angle)
			((unsigned int*)cub->frm_0->data)[f_y * cub->win->x + f_x] =
				get_ceil_pixel(col.angle, cub);
		else if (col.angle > col.f_angle)
			((unsigned int*)cub->frm_0->data)[f_y * cub->win->x + f_x] =
				get_wall_pixel(len_to_wall, col.angle, cub);
		else
			((unsigned int*)cub->frm_0->data)[f_y * cub->win->x + f_x] =
				get_flor_pixel(col.angle, cub);
		col.angle -= col.d_angle;
	}
	if (cub->ray->spr)
		frame_add_sprite(f_x, cub);
}

t_frm_col	frame_s_col_init(float len_to_wall, t_cub *cub)
{
	t_frm_col col;

	col.angle = cub->cam->cam_direction_pitch + cub->cam->cam_angle_pitch / 2;
	col.d_angle = cub->cam->cam_angle_pitch / cub->win->y;
	col.c_angle = atanf((cub->map->blk_z - cub->cam->z) / len_to_wall);
	col.f_angle = atanf((-cub->cam->z) / len_to_wall);
	return (col);
}

t_img		*frame_init(void *mlx_ptr, int x, int y)
{
	t_img	*new_frame;

	if (!(new_frame = (t_img*)malloc(sizeof(t_img))))
		return (NULL);
	if (!(new_frame->ptr = mlx_new_image(mlx_ptr, x, y)))
	{
		free(new_frame);
		return (NULL);
	}
	new_frame->width = x;
	new_frame->height = y;
	if (!(new_frame->data = (unsigned int*)mlx_get_data_addr(new_frame->ptr,
		&(new_frame->btp), &(new_frame->sz_ln), &(new_frame->endian))))
	{
		mlx_destroy_image(mlx_ptr, new_frame->ptr);
		free(new_frame);
		return (NULL);
	}
	return (new_frame);
}
